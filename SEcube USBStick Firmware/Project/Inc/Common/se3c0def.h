/**
  ******************************************************************************
  * File Name          : se3c0def.h
  * Description        : L0 constants
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

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#if defined(CUBESIM)
#include "stubs.h"
#include <time.h>
#endif
#if defined(CUBESIM) && defined(_DEBUG)
#include <stdio.h>
#define SE3_TRACE(msg) printf msg
#else
#define SE3_TRACE(msg)
#endif

#define SE3_CONF_CRC 0

#define SE3_SET64(x, pos, val) do{ memcpy(((uint8_t*)(x))+pos, (void*)&(val), 8); }while(0)
#define SE3_SET32(x, pos, val) do{ memcpy(((uint8_t*)(x))+pos, (void*)&(val), 4); }while(0)
#define SE3_SET16(x, pos, val) do{ memcpy(((uint8_t*)(x))+pos, (void*)&(val), 2); }while(0)
#define SE3_GET64(x, pos, val) do{ memcpy((void*)&(val), ((uint8_t*)(x))+pos, 8); }while(0)
#define SE3_GET32(x, pos, val) do{ memcpy((void*)&(val), ((uint8_t*)(x))+pos, 4); }while(0)
#define SE3_GET16(x, pos, val) do{ memcpy((void*)&(val), ((uint8_t*)(x))+pos, 2); }while(0)

#define SE3_BIT_SET(val, n) do{ val |= (1 << (n)); }while(0)
#define SE3_BIT_CLEAR(val, n) do{ val &= ~(1 << (n)); }while(0)
#define SE3_BIT_TEST(val, n) (val & (1<< (n)))

enum {
	SE3_COMM_BLOCK = 512,
	SE3_COMM_N = 16
};

enum {
	SE3_MAGIC_SIZE = 32,
	SE3_HELLO_SIZE = 32,
	SE3_SERIAL_SIZE = 32
};

/** error codes */
enum {
    SE3_OK = 0,  ///< success
    SE3_ERR_HW = 0xF001,  ///< hardware failure
    SE3_ERR_COMM = 0xF002,  ///< communication error
    SE3_ERR_BUSY = 0xF003,  ///< device locked by another process
    SE3_ERR_STATE = 0xF004,  ///< invalid state for this operation
    SE3_ERR_CMD = 0xF005,  ///< command does not exist
    SE3_ERR_PARAMS = 0xF006,  ///< parameters are not valid
};

/** command codes */
enum {
    SE3_CMD0_FACTORY_INIT = 1,
    SE3_CMD0_ECHO = 2,
	SE3_CMD0_MIX = 3,
	SE3_CMD0_BOOT_MODE_RESET = 4
};

/** command flags */
enum {
    SE3_CMDFLAG_ENCRYPT = (1 << 15),  ///< encrypt packet
    SE3_CMDFLAG_SIGN = (1 << 14) ///< sign payload
};

/** Request fields */
enum {
    SE3_REQ_SIZE_HEADER = 16,
    SE3_REQDATA_SIZE_HEADER = 4,
    SE3_REQ_SIZE_DATA = SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER,
    SE3_REQDATA_SIZE_DATA = SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER,

    SE3_REQ_OFFSET_CMD = 0,
    SE3_REQ_OFFSET_CMDFLAGS = 2,
    SE3_REQ_OFFSET_LEN = 4,
    SE3_REQ_OFFSET_CMDTOKEN = 6,
    SE3_REQ_OFFSET_PADDING = 10,
    SE3_REQ_OFFSET_CRC = 14,
    SE3_REQ_OFFSET_DATA = 16,

    SE3_REQDATA_OFFSET_CMDTOKEN = 0,
    SE3_REQDATA_OFFSET_DATA = 4,
    
    SE3_REQ_MAX_DATA = ((SE3_COMM_BLOCK-SE3_REQ_SIZE_HEADER) + (SE3_COMM_N-2)*(SE3_COMM_BLOCK-SE3_REQDATA_SIZE_HEADER) - 8)
};

/** Response fields */
enum {
    SE3_RESP_SIZE_HEADER = 16,
    SE3_RESPDATA_SIZE_HEADER = 4,
    SE3_RESP_SIZE_DATA = SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER,
    SE3_RESPDATA_SIZE_DATA = SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER,
        
    SE3_RESP_OFFSET_READY = 0,
    SE3_RESP_OFFSET_STATUS = 2,
    SE3_RESP_OFFSET_LEN = 4,
    SE3_RESP_OFFSET_CMDTOKEN = 6,
    SE3_RESP_OFFSET_CRC = 14,
    
    SE3_RESPDATA_OFFSET_CMDTOKEN = 0,
    SE3_RESPDATA_OFFSET_DATA = 4,
    
    SE3_RESP_MAX_DATA = ((SE3_COMM_BLOCK-SE3_REQ_SIZE_HEADER) + (SE3_COMM_N-2)*(SE3_COMM_BLOCK-SE3_REQDATA_SIZE_HEADER) - 8)
};

/** Discover fields */
enum {
    SE3_DISCO_OFFSET_MAGIC = 0,
    SE3_DISCO_OFFSET_SERIAL = 32,
    SE3_DISCO_OFFSET_HELLO = 2*32,
    SE3_DISCO_OFFSET_STATUS = 3*32
};

// required for in-place encryption with AES
#if (SE3_REQ_MAX_DATA % 16 != 0)
#error "SE3_REQ_MAX_DATA is not a multiple of 16"
#endif
#if (SE3_RESP_MAX_DATA % 16 != 0)
#error "SE3_RESP_MAX_DATA is not a multiple of 16"
#endif
