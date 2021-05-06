/**
  ******************************************************************************
  * File Name          : se3_flash.h
  * Description        : Low-level Flash management
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

#ifndef CUBESIM
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#define SE3_FLASH_S0  (FLASH_SECTOR_10)
#define SE3_FLASH_S1  (FLASH_SECTOR_11)
#define SE3_FLASH_S0_ADDR  ((uint32_t)0x080C0000)
#define SE3_FLASH_S1_ADDR  ((uint32_t)0x080E0000)
#define SE3_FLASH_SECTOR_SIZE (128*1024)
#endif

/*
Structure of flash:
	0:31          magic
	32:2047       index
	2048:131071   data

	The data section is divided into 2016 64-byte blocks.
	Each byte of the index stores the type of the corresponding data block.
	A special value (SE3_FLASH_TYPE_CONT) indicates that the block is the continuation of the 
	previous one.
	if the block is invalid, its type is 0. If it has not been written yet, the type is 0xFF.
*/

/** \brief Flash node iterator structure */
typedef struct se3_flash_it_ {
	const uint8_t* addr;
	uint8_t type;
	uint16_t size;
	uint16_t blocks;
	size_t pos;
} se3_flash_it;

typedef struct SE3_FLASH_INFO_ {
    uint32_t sector;  ///< active sector number
    const uint8_t* base;
    const uint8_t* index;
    const uint8_t* data;
    size_t first_free_pos;
    size_t used;
    size_t allocated;
} SE3_FLASH_INFO;

/** Flash nodes' default and reserved types */
enum {
	SE3_FLASH_TYPE_INVALID = 0,  ///< Invalid node
	SE3_FLASH_TYPE_SERIAL = 1,  ///< Device's serial number
	SE3_FLASH_TYPE_CONT = 0xFE,  ///< Continuation of previous node
	SE3_FLASH_TYPE_EMPTY = 0xFF  ///< Not written yet
};

/** Flash fields sizes */
enum {
	SE3_FLASH_MAGIC_SIZE = 32,
	SE3_FLASH_INDEX_SIZE = 2016,
	SE3_FLASH_BLOCK_SIZE = 64,
	SE3_FLASH_NODE_MAX = (4 * 1024),
	SE3_FLASH_NODE_DATA_MAX = (SE3_FLASH_NODE_MAX - 2)
};

/** \brief Initialize flash
 *  
 *  Selects the active flash sector or initializes one
 */
bool se3_flash_init();

/** \brief Initialize flash iterator
 *  
 *  \param it flash iterator structure
 */
void se3_flash_it_init(se3_flash_it* it);

/** \brief Increment flash iterator
 *  
 *  Increment iterator and read information of the next node in flash
 *  \param it flash iterator structure
 *  \return false if end of iteration, else true
 */
bool se3_flash_it_next(se3_flash_it* it);

/** \brief Allocate new node
 *  
 *  Allocates a new node in the flash and points the iterator to the new node.
 *  \remark if a flash operation fails, the hwerror flag (se3c0.hwerror) is set.
 *  \param it flash iterator structure
 *  \param type type of the new flash node
 *  \param size size of the data in the new flash node
 *  \return true if the function succedes, false if there is no more space, or a flash operation fails
 */
bool se3_flash_it_new(se3_flash_it* it, uint8_t type, uint16_t size);

/** \brief Write to flash node
 *  
 *  Write data to flash node.
 *  \remark if a flash operation fails, the hwerror flag (se3c0.hwerror) is set.
 *  \param it flash iterator structure
 *  \param off offset of data
 *  \param data pointer to data to be written
 *  \param size size of data to be written
 */
bool se3_flash_it_write(se3_flash_it* it, uint16_t off, const uint8_t* data, uint16_t size);

/** \brief Delete flash node
 *
 *  Delete a flash node and its data
 *  \remark if a flash operation fails, the hwerror flag (se3c0.hwerror) is set.
 *  \param it flash iterator structure
 *  \return true on success, else false
 */
bool se3_flash_it_delete(se3_flash_it* it);

/** \brief Delete flash node by index
 *
 *  Delete a flash node given its index
 *  \remark if a flash operation fails, the hwerror flag (se3c0.hwerror) is set.
 *  \param pos the index of the node
 *  \return true on success, else false
 */
bool se3_flash_pos_delete(size_t pos);

/** \brief Get unused space
 *
 *  Get unused space in the flash memory, including the space marked as invalid.
 *  If space is available, it does not mean that flash sectors will not be swapped.
 *  \return unused space in bytes
 */
size_t se3_flash_unused();

/** \brief Check if enough space for new node
 *
 *  Check if there is enough space
 *  \param size size of the data to be stored inside the new node
 *  \return true if the node will fit into the flash, else false
 */
bool se3_flash_canfit(size_t size);

/** \brief Initialize flash structures
 *
 *  Initializes the structures for flash management, selecting a sector and its base address.
 *  \param sector active sector number
 *  \param base active sector base address
 */
void se3_flash_info_setup(uint32_t sector, const uint8_t* base);

/** \brief Initialize flash structures
 *
 *  Reset the USEcube device to boot mode by erasing the signature - zeroise.
 *  \param addr signature starting address
 *  \param size signature size
 */
bool se3_flash_bootmode_reset(uint32_t addr, size_t size);


