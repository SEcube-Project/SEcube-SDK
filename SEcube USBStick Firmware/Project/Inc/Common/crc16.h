/**
  ******************************************************************************
  * File Name          : crc16.h
  * Description        : This file contains defines and functions for
  *                      computing CRC
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

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern const uint16_t se3_crc16_table[0x100];
	
	/**
	 *  \brief Compute CRC
	 *  
	 *  \param [in] length Data length
	 *  \param [in] data Data on which CRC is computed
	 *  \param [in] crc CRC
	 *  \return CRC computed
	 *  
	 */
	uint16_t se3_crc16_update(size_t length, const uint8_t* data, uint16_t crc);

#ifdef __cplusplus
}
#endif
