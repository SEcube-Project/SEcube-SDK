/**
  ******************************************************************************
  * File Name          : se3_core.c
  * Description        : Firmware core main functionalities
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

#include "se3_core.h"
#include "se3_communication_core.h"
#include "se3_dispatcher_core.h"
#include "crc16.h"
#include "se3_rand.h"

#define SE3_FLASH_SIGNATURE_ADDR  ((uint32_t)0x08020000)
#define SE3_FLASH_SIGNATURE_SIZE  ((size_t)0x40)

uint8_t se3_sessions_buf[SE3_SESSIONS_BUF];
uint8_t* se3_sessions_index[SE3_SESSIONS_MAX];

void device_init()
{
	se3_communication_core_init();
//	se3_time_init();
	se3_flash_init();
    se3_dispatcher_init();
}

void device_loop()
{
	for (;;) {
		if (comm.req_ready) {
			comm.resp_ready = false;
            se3_cmd_execute();
			comm.req_ready = false;
			comm.resp_ready = true;
		}
	}
}

static uint16_t invalid_cmd_handler(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    return SE3_ERR_CMD;
}

static uint16_t se3_exec(se3_cmd_func handler)
{
    uint16_t resp_size = 0, tmp;
    uint16_t status = SE3_OK;
    uint16_t nblocks = 0;
    uint16_t data_len;
#if SE3_CONF_CRC
	uint16_t crc;
	uint16_t u16tmp;
#endif

    data_len = se3_req_len_data(req_hdr.len);

#if SE3_CONF_CRC
	// compute CRC
	crc = se3_crc16_update(SE3_REQ_OFFSET_CRC, comm.req_hdr, 0);
	if (data_len > 0) {
		crc = se3_crc16_update(data_len, comm.req_data, crc);
	}
	if (req_hdr.crc != crc) {
		status = SE3_ERR_COMM;
		resp_size = 0;
	}
#endif

	if(status == SE3_OK) {
		status = handler(data_len, comm.req_data, &resp_size, comm.resp_data);
	}

    if (hwerror) {
        status = SE3_ERR_HW;
        resp_size = 0;
        hwerror = false;
    }
    else if (resp_size > SE3_RESP_MAX_DATA) {
        status = SE3_ERR_HW;
        resp_size = 0;
    }

    resp_hdr.status = status;

    if (resp_size <= SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) {
        nblocks = 1;
        // zero unused memory
        memset(comm.resp_data + resp_size, 0, SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER - resp_size);
    }
    else {
        tmp = resp_size - (SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER);
        nblocks = 1 + tmp / (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
        if (tmp % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) != 0) {
            nblocks++;
            // zero unused memory
            memset(comm.resp_data + resp_size, 0, (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) - (tmp % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER)));
        }
    }

	resp_hdr.len = se3_resp_len_data_and_headers(resp_size);

#if SE3_CONF_CRC
	u16tmp = 1;
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_READY, u16tmp);
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_STATUS, status);
	SE3_SET16(comm.resp_hdr, SE3_RESP_OFFSET_LEN, resp_hdr.len);
	SE3_SET32(comm.resp_hdr, SE3_RESP_OFFSET_CMDTOKEN, req_hdr.cmdtok[0]);
	crc = se3_crc16_update(SE3_REQ_OFFSET_CRC, comm.resp_hdr, 0);
	if (resp_size > 0) {
		crc = se3_crc16_update(resp_size, comm.resp_data, crc);
	}
	resp_hdr.crc = crc;
#endif

    return nblocks;
}

void se3_cmd_execute()
{
    uint16_t req_blocks = 1, resp_blocks = 1;
    size_t i;
    se3_cmd_func handler = NULL;
	uint32_t cmdtok0;

    req_blocks = req_hdr.len / SE3_COMM_BLOCK;
    if (req_hdr.len % SE3_COMM_BLOCK != 0) {
        req_blocks++;
    }
    if (req_blocks > SE3_COMM_N - 1) {
        // should not happen anyway
        resp_blocks = 0;
        goto update_comm;
    }
    for (i = 1; i < req_blocks; i++) {
        if (req_hdr.cmdtok[i] != req_hdr.cmdtok[i - 1] + 1) {
            resp_blocks = 0;
            goto update_comm;
        }
    }

	if (handler == NULL) {
		switch (req_hdr.cmd) {
		case SE3_CMD0_MIX:
			set_req_hdr(req_hdr);
			handler = dispatcher_call;
			break;
		case SE3_CMD0_ECHO:
			handler = echo;
			break;
		case SE3_CMD0_FACTORY_INIT:
			handler = factory_init;
			break;
		case SE3_CMD0_BOOT_MODE_RESET:
			handler = bootmode_reset;
			break;
		default:
			handler = invalid_cmd_handler;
		}
	}

    resp_blocks = se3_exec(handler);

    // set cmdtok
	cmdtok0 = req_hdr.cmdtok[0];
    for (i = 0; i < resp_blocks; i++) {
        resp_hdr.cmdtok[i] = cmdtok0;
		cmdtok0++;
    }


update_comm:
    // update comm response bit map
    comm.resp_bmap = SE3_BMAP_MAKE(resp_blocks);
}

uint16_t echo(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    memcpy(resp, req, req_size);
    *resp_size = req_size;
    return SE3_OK;
}

uint16_t factory_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    enum {
        OFF_SERIAL = 0
    };
    const uint8_t* serial_tmp = req + OFF_SERIAL;
    se3_flash_it it;
    if(serial.written){
    	return SE3_ERR_STATE;
	}
    se3_flash_it_init(&it);
    if (!se3_flash_it_new(&it, SE3_FLASH_TYPE_SERIAL, SE3_SERIAL_SIZE)) {
        return SE3_ERR_HW;
    }
    if (!se3_flash_it_write(&it, 0, serial_tmp, SE3_SERIAL_SIZE)) {
        return SE3_ERR_HW;
    }
    memcpy(serial.data, serial_tmp, SE3_SERIAL_SIZE);
    serial.written = true;
    return SE3_OK;
}

uint16_t bootmode_reset(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
	if(!(se3_flash_bootmode_reset(SE3_FLASH_SIGNATURE_ADDR, SE3_FLASH_SIGNATURE_SIZE))){
		return SE3_ERR_HW;
	} else {
		return SE3_OK;
	}
}
