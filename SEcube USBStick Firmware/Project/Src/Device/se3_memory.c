/**
  ******************************************************************************
  * File Name          : se3_memory.c
  * Description        : Memory management (session allocator)
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

#include "se3_memory.h"
#include <stdlib.h>

#define SE3_MEM_SIZE_GET(x, val) SE3_GET16(x, 0, (val))
#define SE3_MEM_SIZE_SET(x, val) SE3_SET16(x, 0, (val))
#define SE3_MEM_INFO_GET(x, info) SE3_GET16(x, 2, info)
#define SE3_MEM_INFO_SET(x, info) SE3_SET16(x, 2, info)
#define SE3_MEM_INFO_MAKE(id, valid) ((id) | ((valid)?(1):(0)) << 15 )
#define SE3_MEM_INFO_ISVALID(info) SE3_BIT_TEST(info, 15)
#define SE3_MEM_INFO_ID(info) ((info) & ~(1<<15))

void se3_mem_reset(se3_mem* mem)
{
	size_t i;
	uint16_t tmp;
    mem->used = 0;
    memset(mem->dat, 0, (mem->dat_size)*SE3_MEM_BLOCK);
	
	for (i = 0; i < mem->max_count; i++) {
		mem->ptr[i] = NULL;
	}

    // first entry is an invalid entry filling the whole buffer
    tmp = (uint16_t)mem->dat_size;
    SE3_MEM_SIZE_SET(mem->dat, tmp);
    tmp = SE3_MEM_INFO_MAKE(0, 0);
    SE3_MEM_INFO_SET(mem->dat, tmp);
}

void se3_mem_init(se3_mem* mem, size_t index_size, uint8_t** index, size_t buf_size, uint8_t* buf)
{
    uint16_t nblocks;
	mem->max_count = index_size;
	mem->ptr = index;

    nblocks = (uint16_t)(buf_size / SE3_MEM_BLOCK);
    mem->dat_size = nblocks;
    mem->dat = buf;

    se3_mem_reset(mem);
}

static void se3_mem_compact(uint8_t* p, uint8_t* end)
{
	uint8_t* p2 = p;
	bool empty = false;
    uint16_t size;
    uint16_t info;

	while (p2 < end) {
        SE3_MEM_INFO_GET(p2, info);
        if (SE3_MEM_INFO_ISVALID(info)) {
            break;
        }
        SE3_MEM_SIZE_GET(p2, size);
        p2 += size*SE3_MEM_BLOCK;
		empty = true;
	}
	if (empty) {
        size = (uint16_t)(((uint32_t)(p2 - p)) / SE3_MEM_BLOCK);
		SE3_MEM_SIZE_SET(p, size);
	}
}

static uint8_t* se3_mem_defrag(se3_mem* mem)
{
	uint8_t* p1, *p2;
	uint16_t p2_size;
	bool p2_valid;
	uint16_t id;
	uint8_t* end = mem->dat + mem->dat_size*SE3_MEM_BLOCK;
    uint16_t info, size;

	//find first free block
	p1 = mem->dat;
	while (p1 < end) {
        SE3_MEM_INFO_GET(p1, info);
		if (!SE3_MEM_INFO_ISVALID(info)) {
			break;
		}
        SE3_MEM_SIZE_GET(p1, size);
        p1 += size*SE3_MEM_BLOCK;
	}
	if (p1 >= end) {
		return p1;
	}

    SE3_MEM_SIZE_GET(p1, size);
	p2 = p1 + size*SE3_MEM_BLOCK;
	while(p2<end) {
        SE3_MEM_SIZE_GET(p2, p2_size);
        SE3_MEM_INFO_GET(p2, info);
        p2_valid = SE3_MEM_INFO_ISVALID(info);
		if ( p2_valid ) {
			memmove(p1, p2, p2_size*SE3_MEM_BLOCK);
			p1 += p2_size*SE3_MEM_BLOCK;
		}
		p2 += p2_size*SE3_MEM_BLOCK;
	}
	if (p1 < end) {
        size = (uint16_t)(((uint32_t)(end - p1))/SE3_MEM_BLOCK);
		SE3_MEM_SIZE_SET(p1, size);

        // clear valid
        SE3_MEM_INFO_GET(p1, info);
        info = SE3_MEM_INFO_MAKE(SE3_MEM_INFO_ID(info), 0);
        SE3_MEM_INFO_SET(p1, info);
	}
	
	//rebuild ptr table
	
	p2 = mem->dat;
	while (p2 < end) {
        SE3_MEM_INFO_GET(p2, info);
		if (SE3_MEM_INFO_ISVALID(info)) {
			id = SE3_MEM_INFO_ID(info);
			mem->ptr[id] = p2;
		}
        SE3_MEM_SIZE_GET(p2, size);
        p2 += size*SE3_MEM_BLOCK;
	}

	//return first empty block;
	return p1;
}

int32_t se3_mem_alloc(se3_mem* mem, size_t size)
{
	uint8_t* p = mem->dat, *p2;
	uint8_t* dat_end = mem->dat + (mem->dat_size)*SE3_MEM_BLOCK;
	size_t i;
    uint16_t p_info;
	uint16_t p_size;
    uint16_t u16tmp;
    uint16_t nblocks;
	bool valid;

	// header size
	size += SE3_MEM_HEADER;

    // number of blocks
    nblocks = (uint16_t)(size / SE3_MEM_BLOCK);
    if (size % SE3_MEM_BLOCK != 0) {
        nblocks++;
    }
    
	if (mem->used + nblocks > mem->dat_size) {
		//no more space
		return -1;
	}
	
	for (i = 0; i < mem->max_count; i++) {
		if (mem->ptr[i] == NULL) {
			break;
		}
	}
	if (i >= mem->max_count) {
		// no more slots
		return -1;
	}
	while (p < dat_end) {
        SE3_MEM_SIZE_GET(p, p_size);
        SE3_MEM_INFO_GET(p, p_info);
		valid = SE3_MEM_INFO_ISVALID(p_info);
		if (p_size == 0 ) {
			SE3_TRACE(("E mem_alloc memory corrupted, block with size=0\n"));
			return -1;
		}
		else if (!valid) {
			// try to aggregate blocks ahead
            if (p_size < nblocks) {
				se3_mem_compact(p, dat_end);
			}
            
			if (p_size == nblocks) {
                // ok, no need to resize entry
				break;
			}
            else if (p_size > nblocks) {
                // ok, resize entry
                p2 = p + nblocks*SE3_MEM_BLOCK;
                u16tmp = p_size - nblocks;
                SE3_MEM_SIZE_SET(p2, u16tmp);
                u16tmp = SE3_MEM_INFO_MAKE(0, 0);
                SE3_MEM_INFO_SET(p2, u16tmp);
                break;
            }
		}
        p += p_size*SE3_MEM_BLOCK;
	}
	if (p >= dat_end) {
        // there enough free memory but it is fragmented
		p = se3_mem_defrag(mem);
		SE3_TRACE(("[se3_mem_alloc] defragging session memory\n"));

		if (p < dat_end) {
			SE3_MEM_SIZE_GET(p, p_size);
			p2 = p + nblocks*SE3_MEM_BLOCK;
			u16tmp = p_size - nblocks;
			SE3_MEM_SIZE_SET(p2, u16tmp);
			u16tmp = SE3_MEM_INFO_MAKE(0, 0);
			SE3_MEM_INFO_SET(p2, u16tmp);
		}
		else {
			SE3_TRACE(("[se3_mem_alloc] memory corrupted, usage reports free memory while none\n"));
			return -1;
		}
	}

    // update entry header
    SE3_MEM_SIZE_SET(p, nblocks);
    u16tmp = SE3_MEM_INFO_MAKE((uint16_t)i, 1);
    SE3_MEM_INFO_SET(p, u16tmp);

    // update index
	mem->ptr[i] = p;
	(mem->used) += nblocks;

	return (int32_t)i;
}

uint8_t* se3_mem_ptr(se3_mem* mem, int32_t id)
{
    if (id < 0) {
        return NULL;
    }
	if ((uint32_t)id < mem->max_count) {
		if (mem->ptr[id] == NULL) {
			SE3_TRACE(("E mem_ptr index points to NULL\n"));
			return NULL;
		}
        return mem->ptr[id] + SE3_MEM_HEADER;
	}
	else {
		SE3_TRACE(("E mem_ptr index out of range\n"));
	}
	return NULL;
}

void se3_mem_free(se3_mem* mem, int32_t id)
{
	uint8_t* p;
    uint16_t info;
    uint16_t size;
    if (id < 0) {
        return;
    }
	if ((uint32_t)id < mem->max_count) {
		p = mem->ptr[id];
		mem->ptr[id] = NULL;
        SE3_MEM_SIZE_GET(p, size);
        SE3_MEM_INFO_GET(p, info);
        info = SE3_MEM_INFO_MAKE(SE3_MEM_INFO_ID(info), 0);
        SE3_MEM_INFO_SET(p, info);
        (mem->used) -= size;
	}
}


