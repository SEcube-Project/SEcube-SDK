/**
  ******************************************************************************
  * File Name          : se3_communication_core.c
  * Description        : USB read/write handlers
  ******************************************************************************
  *
  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#include "se3_communication_core.h"
#ifndef CUBESIM
#include <se3_sdio.h>
#endif

SE3_COMM_STATUS comm;
se3_comm_req_header req_hdr;
se3_comm_resp_header resp_hdr;

uint8_t se3_comm_request_buffer[SE3_COMM_N*SE3_COMM_BLOCK];
uint8_t se3_comm_response_buffer[SE3_COMM_N*SE3_COMM_BLOCK];
const uint8_t se3_hello[SE3_HELLO_SIZE] = {
	'H', 'e', 'l', 'l', 'o', ' ', 'S', 'E',
    'c', 'u', 'b', 'e', 0, 0, 0, 0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

enum s3_storage_range_direction {
	range_write, range_read
};

/** \brief SDIO read/write request buffer context */
typedef struct s3_storage_range_ {
	uint8_t* buf;
	uint32_t first;
	uint32_t count;
} s3_storage_range;

/**\brief Initializes the communication core structures */
void se3_communication_core_init()
{
	memset(&comm, 0, sizeof(SE3_COMM_STATUS));
	memset(&req_hdr, 0, sizeof(se3_comm_req_header));
	memset(&resp_hdr, 0, sizeof(se3_comm_resp_header));
	memset(&serial, 0, sizeof(SE3_SERIAL));

    comm.req_hdr = se3_comm_request_buffer;
    comm.req_data = se3_comm_request_buffer + SE3_REQ_SIZE_HEADER;
    comm.resp_hdr = se3_comm_response_buffer;
    comm.resp_data = se3_comm_response_buffer + SE3_RESP_SIZE_HEADER;
    comm.magic_bmap = SE3_BMAP_MAKE(16);
    comm.magic_ready = false;
    comm.req_bmap = SE3_BMAP_MAKE(1);
    comm.locked = false;
    comm.req_ready = false;
    comm.req_bmap = SE3_BMAP_MAKE(32);
    comm.resp_ready = true;
    comm.resp_bmap = 0;
}


/** \brief Check if block contains the magic sequence
 *  \param buf pointer to block data
 *  \return true if the block contains the magic sequence, otherwise false
 *
 *  Check if a block of data contains the magic sequence, used to initialize the special
 *    protocol file.
 */
static bool block_is_magic(const uint8_t* buf)
{
	const uint8_t* a = buf;
	const uint8_t* b = se3_magic;
	size_t i;
	for (i = 0; i < SE3_COMM_BLOCK / SE3_MAGIC_SIZE - 1; i++) {
		if (memcmp(a, b, SE3_MAGIC_SIZE))return false;
        a += SE3_MAGIC_SIZE;
	}
	if (buf[SE3_COMM_BLOCK - 1] >= SE3_COMM_N)return false;
	return true;
}

/** \brief Check if block belongs to the special protocol file
 *  \param block block number
 *  \return the index of the corresponding protocol file block, or -1 if the block does not
 *    belong to the protocol file.
 *  
 *  The special protocol file is made up of multiple blocks. Each block is mapped to a block
 *    on the physical storage
 */
static int find_magic_index(uint32_t block)
{
	int i; size_t k;
	for (i = 0, k = comm.block_guess; i < SE3_COMM_N; i++, k = (k+1)%(SE3_COMM_N) ) {
		if (block == comm.blocks[i]) {
			comm.block_guess = (size_t)((i + 1) % 16);
			return i;
		}
	}
	return -1;
}


/** \brief add request to SDIO read/write buffer
 *  \param range context; the count field must be initialized to zero on first usage
 *  \param lun parameter from USB handler
 *  \param buf pointer to request data
 *  \param block request block index
 *  \param direction read or write
 *  
 *  Contiguous requests are processed with a single call to the SDIO interface, as soon as
 *    a non-contiguous request is added.
 */
static int32_t se3_storage_range_add(s3_storage_range* range, uint8_t lun, uint8_t* buf, uint32_t block, enum s3_storage_range_direction direction)
{
	bool ret = true;
	if (range->count == 0) {
		range->buf = buf;
		range->first = block;
		range->count = 1;
	}
	else {
		if (block == range->first + range->count) {
			range->count++;
		}
		else {
			if (direction == range_write){
				ret = secube_sdio_write(lun, range->buf, range->first, range->count);
				SE3_TRACE(("%i: write buf=%u count=%u to block=%u", ret, (unsigned)range->buf, range->count, range->first));
			}
			else {
				ret = secube_sdio_read(lun, range->buf, range->first, range->count);
				SE3_TRACE(("%d: read buf=%u count=%u from block=%u", ret, (unsigned)range->buf, range->count, range->first));
			}
			range->count = 0;
		}
	}

	return (ret)?(SE3_PROTO_OK):(SE3_PROTO_FAIL);
}

/** \brief Reset protocol request buffer
 *  
 *  Reset the protocol request buffer, making the device ready for a new request.
 */
void se3_proto_request_reset()
{
    comm.req_ready = false;
    comm.req_bmap = SE3_BMAP_MAKE(32);
}

/** \brief Handle request for incoming protocol block
 *  \param index index of block in the special protocol file
 *  \param blockdata data
 *  
 *  Handle a single block belonging to a protocol request. The data is stored in the
 *    request buffer. As soon as the request data is received completely, the device
 *    will start processing the request
 */
static void handle_req_recv(int index, const uint8_t* blockdata)
{
    uint16_t nblocks;
    if (index == SE3_COMM_N - 1) {
        SE3_TRACE(("P data write to block %d ignored", index));
        return;
    }

    comm.resp_ready = false;

    if (index == 0) {
        // REQ block

        // read and decode header
        memcpy(comm.req_hdr, blockdata, SE3_REQ_SIZE_HEADER);
        SE3_GET16(comm.req_hdr, SE3_REQ_OFFSET_CMD, req_hdr.cmd);
        SE3_GET16(comm.req_hdr, SE3_REQ_OFFSET_CMDFLAGS, req_hdr.cmd_flags);
        SE3_GET16(comm.req_hdr, SE3_REQ_OFFSET_LEN, req_hdr.len);
        SE3_GET32(comm.req_hdr, SE3_REQ_OFFSET_CMDTOKEN, req_hdr.cmdtok[0]);
#if SE3_CONF_CRC
		SE3_GET16(comm.req_hdr, SE3_REQ_OFFSET_CRC, req_hdr.crc);
#endif
        // read data
        memcpy(comm.req_data, blockdata + SE3_REQ_SIZE_HEADER, SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);

        nblocks = req_hdr.len / SE3_COMM_BLOCK;
        if (req_hdr.len%SE3_COMM_BLOCK != 0) {
            nblocks++;
        }
        if (nblocks > SE3_COMM_N - 1) {
            resp_hdr.status = SE3_ERR_COMM;
            comm.req_bmap = 0;
            comm.resp_ready = true;
        }
        // update bit map
        comm.req_bmap &= SE3_BMAP_MAKE(nblocks);
        SE3_BIT_CLEAR(comm.req_bmap, 0);
    }
    else {
        // REQDATA block
        // read header
        SE3_GET32(blockdata, SE3_REQDATA_OFFSET_CMDTOKEN, req_hdr.cmdtok[index]);
        // read data
        memcpy(
            comm.req_data + 1 * (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER) + (index - 1)*(SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER),
            blockdata + SE3_REQDATA_SIZE_HEADER,
            SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER);
        SE3_GET32(blockdata, 0, req_hdr.cmdtok[index]);
        // update bit map
        SE3_BIT_CLEAR(comm.req_bmap, index);
    }

    if (comm.req_bmap == 0) {
        comm.req_ready = true;
        comm.req_bmap = SE3_BMAP_MAKE(32);
        comm.block_guess = 0;
    }
}



/**	User-written USB interface that implements the write operation of the
  *	driver; it forwards the data on the SD card if the data block does not
  *	 contain the magic sequence, otherwise the data block is unpacked for further
  *	 elaborations;
  */
int32_t se3_proto_recv(uint8_t lun, const uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	int32_t r = SE3_PROTO_OK;
	uint32_t block;
	int index;
	const uint8_t* data = buf;

	s3_storage_range range = {
		.first = 0,
		.count = 0
	};

	for (block = blk_addr; block < blk_addr + blk_len; block++) {
		if (block == 0) {
			r = se3_storage_range_add(&range, lun, (uint8_t*)data, block, range_write);
			if (SE3_PROTO_OK != r) return r;
		}
		else {
            if (block_is_magic(data)) {
                // magic block
                if (comm.locked) {
                    // if locked, prevent initialization
                    continue;
                }
                if (comm.magic_ready) {
                    // if magic already initialized, reset
                    comm.magic_ready = false;
                    comm.magic_bmap = SE3_BMAP_MAKE(16);
                    for (index = 0; index < 16; index++)
                        comm.blocks[index] = 0;
                }
                // store block in blocks map
                index = data[SE3_COMM_BLOCK - 1];
                comm.blocks[index] = block;
                SE3_BIT_CLEAR(comm.magic_bmap, index);
                if (comm.magic_bmap == 0) {
                    comm.magic_ready = true;
                }
            }
            else{
                // not a magic block
                if (!comm.magic_ready) {
                    // magic file has not been written yet. forward
                    r = se3_storage_range_add(&range, lun, (uint8_t*)data, block, range_write);
                    if (SE3_PROTO_OK != r) return r;
                }
                else {
                    // magic file has been written. may be a command
                    index = find_magic_index(block);
                    if (index == -1) {
                        // block is not a request. forward
                        r = se3_storage_range_add(&range, lun, (uint8_t*)data, block, range_write);
                        if (SE3_PROTO_OK != r) return r;
                    }
                    else {
                        // block is a request
                        if (comm.req_ready) {
                            // already processing request. ignore
                            SE3_TRACE(("P W%02u request already fully received", (unsigned)index));
                            continue;
                        }
                        else {
                            handle_req_recv(index, data);
                        }
                    }
                }
            }
		}
		data += SE3_COMM_BLOCK;
	}

	//flush any remaining block
	return se3_storage_range_add(&range, lun, NULL, 0xFFFFFFFF, range_write);
}

/** \brief Handle request for outgoing protocol block
 *  \param index index of block in the special protocol file
 *  \param blockdata output data
 *
 *  Output a single block of a protocol response. If the response is ready,
 *    the data is taken from the response buffer. Otherwise the 'not ready' state is
 *    returned.
 */
static void handle_resp_send(int index, uint8_t* blockdata)
{
    uint16_t u16tmp;
    
    if (index == SE3_COMM_N - 1) {
        // discover
        memcpy(blockdata + SE3_DISCO_OFFSET_MAGIC, se3_magic + SE3_MAGIC_SIZE / 2, SE3_MAGIC_SIZE / 2);
        memcpy(blockdata + SE3_DISCO_OFFSET_MAGIC + SE3_MAGIC_SIZE / 2, se3_magic, SE3_MAGIC_SIZE / 2);
        memcpy(blockdata + SE3_DISCO_OFFSET_SERIAL, serial.data, SE3_SERIAL_SIZE);
        memcpy(blockdata + SE3_DISCO_OFFSET_HELLO, se3_hello, SE3_HELLO_SIZE);
        u16tmp = (comm.locked) ? (1) : (0);
        SE3_SET16(blockdata, SE3_DISCO_OFFSET_STATUS, u16tmp);
    }
    else {
        if (comm.resp_ready) {
            // response ready
            if (SE3_BIT_TEST(comm.resp_bmap, index)) {
                // read valid block
                if (index == 0) {
                    // RESP block

                    // encode and write header
                    u16tmp = 1;
                    SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_READY, u16tmp);
                    SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_STATUS, resp_hdr.status);
                    SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_LEN, resp_hdr.len);
                    SE3_SET32(comm.resp_hdr, SE3_RESP_OFFSET_CMDTOKEN, resp_hdr.cmdtok[0]);
#if SE3_CONF_CRC
                    SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_CRC, resp_hdr.crc);
#endif
                    memcpy(blockdata, comm.resp_hdr, SE3_RESP_SIZE_HEADER);

                    // write data
                    memcpy(blockdata + SE3_RESP_SIZE_HEADER, comm.resp_data, SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER);
                }
                else {
                    // RESPDATA block
                    // write header
                    SE3_SET32(blockdata, SE3_RESPDATA_OFFSET_CMDTOKEN, resp_hdr.cmdtok[index]);
                    // write data
                    memcpy(
                        blockdata + SE3_RESPDATA_SIZE_HEADER,
                        comm.resp_data + 1 * (SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) + (index - 1)*(SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER),
                        SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
                }
            }
            else {
                // read invalid block
                memset(blockdata, 0, SE3_COMM_BLOCK);
            }
        }
        else {
            // response not ready
            memset(blockdata, SE3_RESP_OFFSET_READY, sizeof(uint16_t));
        }
    }
}



/*	User-written USB interface that implements the read operation of the
 * 	driver; it sends the data on the SD card if the data block does not
 *	contain the magic sequence, otherwise it handles the proto request.
 */
int32_t se3_proto_send(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	int32_t r = SE3_PROTO_OK;
	uint32_t block;
	int index;
	uint8_t* data = buf;
	s3_storage_range range = {
		.first = 0,
		.count = 0
	};

	for (block = blk_addr; block < blk_addr + blk_len; block++) {
		if(block==0) {
            // forward
			if (r == SE3_PROTO_OK) r = se3_storage_range_add(&range, lun, data, block, range_read);
		}
		else{
			index = find_magic_index(block);
            if (index == -1) {
                // forward
                if (r == SE3_PROTO_OK) r = se3_storage_range_add(&range, lun, data, block, range_read);
            }
            else {
                handle_resp_send(index, data);
            }
		}
		data += SE3_COMM_BLOCK;
	}

	//flush any remaining block
    if (r == SE3_PROTO_OK) r = se3_storage_range_add(&range, lun, NULL, 0xFFFFFFFF, range_read);
    return r;
}


