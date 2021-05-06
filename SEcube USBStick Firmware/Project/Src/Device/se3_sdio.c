/**
  ******************************************************************************
  * File Name          : se3_sdio.c
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

#include "se3_sdio.h"
#include "usbd_storage_if.h"
#include "sdio.h"


bool secube_sdio_write(uint8_t lun, const uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (HAL_SD_WriteBlocks_DMA(&hsd, (uint32_t *)buf, blk_addr * STORAGE_BLK_SIZ, STORAGE_BLK_SIZ, blk_len) == SD_OK)
		if (HAL_SD_CheckWriteOperation(&hsd, (uint32_t)SD_DATATIMEOUT) == SD_OK)
			return true;

	return false;
}
bool secube_sdio_read(uint8_t lun, uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (HAL_SD_ReadBlocks_DMA(&hsd, (uint32_t *)buf, blk_addr * STORAGE_BLK_SIZ, STORAGE_BLK_SIZ, blk_len) == SD_OK)
		if (HAL_SD_CheckReadOperation(&hsd, (uint32_t)SD_DATATIMEOUT) == SD_OK)
			return true;

	return false;
}

bool secube_sdio_capacity(uint32_t *block_num, uint16_t *block_size)
{

	if (HAL_SD_Get_CardInfo(&hsd,&SDCardInfo) == SD_OK){
			*block_num = (SDCardInfo.CardCapacity)/STORAGE_BLK_SIZ  - 1;
			*block_size = STORAGE_BLK_SIZ;
			return true;
		}

	return false;
}

bool secube_sdio_isready(void)
{

	if (HAL_SD_GetStatus(&hsd) != SD_TRANSFER_OK)
		return false;

	return true;
}
