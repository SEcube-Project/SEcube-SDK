/**
  ******************************************************************************
  * File Name          : se3_sdio.h
  * Description        : SEcube MicroSD interface
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
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SD_DATATIMEOUT ((uint32_t)100000000)
#define MEM_OP_FAIL (1)
#define MEM_OP_OK (0)
#define STORAGE_LUN_NBR (1)
#define STORAGE_BLK_SIZ (512)

bool secube_sdio_read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
bool secube_sdio_write(uint8_t lun, const uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
bool secube_sdio_capacity(uint32_t *block_num, uint16_t *block_size);
bool secube_sdio_isready(void);

