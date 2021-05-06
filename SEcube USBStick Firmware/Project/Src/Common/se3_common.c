/**
  ******************************************************************************
  * File Name          : se3_common.c
  * Description        : Common functions and data structures.
  *                      Debug tools are also here
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

#include "se3_common.h"
#include "se3_sdio.h"
#include <string.h>

const uint8_t se3_magic[SE3_MAGIC_SIZE] = {
    0x3c, 0xab, 0x78, 0xb6, 0x2, 0x64, 0x47, 0xe9, 0x30, 0x26, 0xd4, 0x1f, 0xad, 0x68, 0x22, 0x27,
    0x41, 0xa4, 0x32, 0xba, 0xbe, 0x54, 0x83, 0xee, 0xab, 0x6b, 0x62, 0xce, 0xf0, 0x5c, 0x7, 0x91
};

SE3_SERIAL serial;
uint16_t hwerror;

uint16_t se3_req_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_REQ_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers/SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_REQ_SIZE_HEADER - (nblocks - 1)*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_req_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER) {
        return len_data + SE3_REQ_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_RESP_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers / SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_RESP_SIZE_HEADER - (nblocks - 1)*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) {
        return len_data + SE3_RESP_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_nblocks(uint16_t len)
{
    uint16_t nblocks = len / SE3_COMM_BLOCK;
    if (len%SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    return nblocks;
}

