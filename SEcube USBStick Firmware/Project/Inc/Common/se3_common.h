/**
  ******************************************************************************
  * File Name          : se3_common.h
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


#pragma once

#include "se3c1def.h"
#include "se3_sdio.h"

extern const uint8_t se3_magic[SE3_MAGIC_SIZE];

#ifndef se3_serial_def
#define se3_serial_def
typedef struct SE3_SERIAL_ {
    uint8_t data[SE3_SERIAL_SIZE];
    bool written;  					///< Indicates whether the serial number has been set (by FACTORY_INIT)
} SE3_SERIAL;
#endif

/** \brief decoded request header */
typedef struct se3_comm_req_header_ {
    uint16_t cmd;
    uint16_t cmd_flags;
    uint16_t len;
#if SE3_CONF_CRC
    uint16_t crc;
#endif
    uint32_t cmdtok[SE3_COMM_N - 1];
} se3_comm_req_header;

extern SE3_SERIAL serial;
extern uint16_t hwerror;

/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_req_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a request accounting for headers
 *  
 *  \param [in] len_data Data length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_req_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_resp_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a response accounting for headers
 *  
 *  \param [in] len_data Data Length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_resp_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute number of SE3_COMM_BLOCK blocks, given length in Bytes
 *  
 *  \param [in] cap Length
 *  \return Number of Blocks
 *  
 */
uint16_t se3_nblocks(uint16_t len);
