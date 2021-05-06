/**
  ******************************************************************************
  * File Name          : se3_flash.c
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

#include "se3_flash.h"
#include "se3_common.h"

SE3_FLASH_INFO flash;

static bool flash_fill(uint32_t addr, uint8_t val, size_t size)
{
	bool success = true;
	HAL_FLASH_Unlock();
	while (size) {
		if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, (uint64_t)val)) {
			success = false;
            hwerror = true;
			break;
		}
		size--;
		addr++;
	}
	HAL_FLASH_Lock();
	return success;
}

static bool flash_zero(uint32_t addr, size_t size)
{
	bool success = true;
	HAL_FLASH_Unlock();
	while (size) {
		if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, 0)) {
			success = false;
            hwerror = true;
			break;
		}
		size--;
		addr++;
	}
	HAL_FLASH_Lock();
	return success;
}

static bool flash_program(uint32_t addr, const uint8_t* data, size_t size)
{
	bool success = true;
	HAL_FLASH_Unlock();
	while (size) {
		if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, (uint64_t)*data)) {
			success = false;
			break;
		}
		size--;
		addr++;
		data++;
	}
	HAL_FLASH_Lock();
    if (!success) {
        hwerror = true;
    }
	return success;
}

static bool flash_erase(uint32_t sector) {
    bool success = true;
#ifdef CUBESIM
    memset((sector == SE3_FLASH_S0) ? (uint8_t*)(SE3_FLASH_S0_ADDR) : (uint8_t*)(SE3_FLASH_S1_ADDR), 0xFF, SE3_FLASH_SECTOR_SIZE);
#else
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;
	HAL_StatusTypeDef result;
	
	HAL_FLASH_Unlock();

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = sector;
	EraseInitStruct.NbSectors = 1;
	result = HAL_FLASHEx_Erase(&EraseInitStruct, (uint32_t*)&SectorError);
	if (result != HAL_OK){
        success = false;
        hwerror = true;
    }
	HAL_FLASH_Lock();
#endif
    return success;
}

static bool flash_swap()
{
	uint32_t other;
	uint32_t other_base;
	uint32_t other_index;
	size_t other_used;
	size_t other_pos;

	size_t n;
	bool success, b;
	se3_flash_it it;
	if (flash.sector == SE3_FLASH_S0) {
		other = SE3_FLASH_S1;
		other_base = SE3_FLASH_S1_ADDR;
	}
	else if (flash.sector == SE3_FLASH_S1) {
		other = SE3_FLASH_S0;
		other_base = SE3_FLASH_S0_ADDR;
	}
	else {
		return false;
	}
	other_index = other_base + SE3_FLASH_MAGIC_SIZE;
	//erase other sector
	flash_erase(other);
	//zero non-programmed slots in index table (first_free_pos to end)
	if (flash.first_free_pos < SE3_FLASH_INDEX_SIZE) {
		n = SE3_FLASH_INDEX_SIZE - flash.first_free_pos;
		flash_zero((uint32_t)flash.index + flash.first_free_pos, n);
	}

	//copy good blocks to other sector
	success = true;
	other_used = SE3_FLASH_MAGIC_SIZE + SE3_FLASH_INDEX_SIZE;
	other_pos = 0;
	se3_flash_it_init(&it);	
	while (se3_flash_it_next(&it)) {
		if (it.type != SE3_FLASH_TYPE_INVALID) {
			//copy data
			b = flash_program(
				other_base + other_used, 
				flash.data + it.pos*SE3_FLASH_BLOCK_SIZE,
				it.blocks*SE3_FLASH_BLOCK_SIZE
			);
			if (!b) {
				success = false; break;
			}

			//write index
			b = flash_program(other_index + other_pos, &(it.type), 1);
			if (!b) {
				success = false; break;
			}
			if (it.blocks > 1) {
				b = flash_fill(other_index + other_pos + 1, 0xFE, it.blocks - 1);
				if (!b) {
					success = false; break;
				}
			}

			other_used += it.blocks*SE3_FLASH_BLOCK_SIZE;
			other_pos += it.blocks;
		}
	}
	if (!success) {
		return false;
	}

	//write magic to other sector
	if (!flash_program(other_base, se3_magic, SE3_FLASH_MAGIC_SIZE)) {
		return false;
	}

	//clear magic from this sector
	if (!flash_zero((uint32_t)flash.base, 1)) {
		return false;
	}

	//swap sectors
	flash.base = (uint8_t*)other_base;
    flash.sector = other;
    flash.index = flash.base + SE3_FLASH_MAGIC_SIZE;
    flash.data = flash.index + SE3_FLASH_INDEX_SIZE;
    flash.allocated = flash.used = other_used;
    flash.first_free_pos = other_pos;

	return true;
}

void se3_flash_info_setup(uint32_t sector, const uint8_t* base)
{
	flash.base = base;
    flash.sector = sector;
    flash.index = flash.base + SE3_FLASH_MAGIC_SIZE;
    flash.data = flash.index + SE3_FLASH_INDEX_SIZE;
    flash.allocated = flash.used = SE3_FLASH_MAGIC_SIZE + SE3_FLASH_INDEX_SIZE;
    flash.first_free_pos = 0;
}

bool se3_flash_canfit(size_t size)
{
	size_t size_on_flash = size + 2;
	return (size_on_flash <= (SE3_FLASH_SECTOR_SIZE - flash.used));
}

bool se3_flash_init()
{
	se3_flash_it it;
	uint8_t* base;
	uint32_t sector;
	//uint16_t record_key;

	// check for flash magic
	bool magic0 = !memcmp((void*)SE3_FLASH_S0_ADDR, se3_magic, SE3_FLASH_MAGIC_SIZE);
	bool magic1 = !memcmp((void*)SE3_FLASH_S1_ADDR, se3_magic, SE3_FLASH_MAGIC_SIZE);
	
	//choose active sector
	if (magic0 && magic1) {
		//both marked, the one with last index programmed should be deleted
		if (0xFF == *((uint8_t*)(SE3_FLASH_S1_ADDR + SE3_FLASH_MAGIC_SIZE + SE3_FLASH_INDEX_SIZE - 1))){
			magic0 = false;
			flash_zero(SE3_FLASH_S0_ADDR, 1);
		}
		else {
			magic1 = false;
			flash_zero(SE3_FLASH_S1_ADDR, 1);
		}
	}

	if (magic0) {
		base = (uint8_t*)SE3_FLASH_S0_ADDR;
		sector = SE3_FLASH_S0;
	}
	else if (magic1) {
		base = (uint8_t*)SE3_FLASH_S1_ADDR;
		sector = SE3_FLASH_S1;
	}
	else {
		// initialize S0 as active sector
		flash_erase(SE3_FLASH_S0);
		flash_program(SE3_FLASH_S0_ADDR, se3_magic, SE3_FLASH_MAGIC_SIZE);
		base = (uint8_t*)SE3_FLASH_S0_ADDR;
		sector = SE3_FLASH_S0;
	}
	se3_flash_info_setup(sector, base);

	//scan flash
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)) {
		flash.allocated += it.blocks*SE3_FLASH_BLOCK_SIZE;
		if (it.type != 0) {
			flash.used += it.blocks*SE3_FLASH_BLOCK_SIZE;
            if (it.type == SE3_FLASH_TYPE_SERIAL) {
                memcpy(serial.data, it.addr, SE3_SERIAL_SIZE);
                serial.written = true;
            }
		}
	}
	if (it.pos > SE3_FLASH_INDEX_SIZE) {
		it.pos = SE3_FLASH_INDEX_SIZE;
	}
	flash.first_free_pos = it.pos;

	return true;
}

bool se3_flash_it_write(se3_flash_it* it, uint16_t off, const uint8_t* data, uint16_t size)
{
	if (off + size > 2 + it->size)return false;
    return flash_program((uint32_t)it->addr + off, data, size);
}

void se3_flash_it_init(se3_flash_it* it)
{
	it->addr = NULL;
}

bool se3_flash_it_next(se3_flash_it* it)
{
	uint8_t type;
	const uint8_t* node;
	size_t pos2;
	if (it->addr == NULL) {
		it->pos = 0;
		it->addr = flash.data + 2;
	}
	else {
		(it->pos)+=it->blocks;
	}
	while (it->pos < SE3_FLASH_INDEX_SIZE) {
		type = *(flash.index + it->pos);
		if (type == 0xFF) return false;
		if (type != 0xFE) {
			node = flash.data + (it->pos) * SE3_FLASH_BLOCK_SIZE;
			it->addr = node + 2;
            SE3_GET16(node, 0, it->size);
			it->type = type;

			//count 'CONT' nodes after
			pos2 = it->pos + 1;
			while (pos2 < SE3_FLASH_INDEX_SIZE && *(flash.index + pos2) == 0xFE)pos2++;
			it->blocks = (uint16_t)(pos2 - it->pos);
			return true;
		}
		(it->pos)++;
	}
	return false;
}

size_t se3_flash_unused()
{
	return SE3_FLASH_SECTOR_SIZE - flash.used;
}

bool se3_flash_it_new(se3_flash_it* it, uint8_t type, uint16_t size)
{
	size_t pos, nblocks;
	const uint8_t* node;
	size_t avail = SE3_FLASH_SECTOR_SIZE - flash.allocated;
	uint16_t size_on_flash = size + 2;
	if (size_on_flash > SE3_FLASH_NODE_MAX)return false;
	if (size_on_flash > (SE3_FLASH_SECTOR_SIZE - flash.used)) {
		return false;
	}
	if (size_on_flash > avail) {
		// swap sector
		if (!flash_swap()) {
			return false;
		}
	}
	if (flash.first_free_pos >= SE3_FLASH_INDEX_SIZE) {
		return false;
	}
	pos = flash.first_free_pos;
	node = flash.data + pos*SE3_FLASH_BLOCK_SIZE;

	nblocks = size_on_flash / SE3_FLASH_BLOCK_SIZE;
	if (size_on_flash % SE3_FLASH_BLOCK_SIZE)nblocks++;
	if (!flash_program((uint32_t)flash.index + pos, &type, 1)) {
		return false;
	}
	flash.first_free_pos += 1;
	if (nblocks > 1) {
		if (!flash_fill((uint32_t)flash.index + pos + 1, 0xFE, nblocks - 1)) {
			return false;
		}
		flash.first_free_pos += nblocks - 1;
	}
	
	if (!flash_program((uint32_t)node, (uint8_t*)&size, 2)) {
		return false;
	}
	it->addr = node + 2;
	it->pos = pos;
	it->size = size;
	it->type = type;
	it->blocks = (uint16_t)nblocks;

	flash.used += nblocks*SE3_FLASH_BLOCK_SIZE;
	flash.allocated += nblocks*SE3_FLASH_BLOCK_SIZE;

	return true;
}

bool se3_flash_pos_delete(size_t pos)
{
	size_t pos2, blocks;
	if (pos >= SE3_FLASH_INDEX_SIZE)return false;
	pos2 = pos + 1;
	while (pos2 < SE3_FLASH_INDEX_SIZE && *(flash.index + pos2) == 0xFE)pos2++;
	blocks = (pos2 - pos);
	if (pos + blocks > SE3_FLASH_INDEX_SIZE)return false;
	if (!flash_zero((uint32_t)flash.index + pos, blocks)) {
		return false;
	}
	flash.used -= blocks*SE3_FLASH_BLOCK_SIZE;
	return true;
}

bool se3_flash_it_delete(se3_flash_it* it)
{
	if (it->pos + it->blocks > SE3_FLASH_INDEX_SIZE) {
		return false;
	}
	if (!flash_zero((uint32_t)flash.index + it->pos, it->blocks)) {
		return false;
	}
	flash.used -= it->blocks*SE3_FLASH_BLOCK_SIZE;
	return true;
}

bool se3_flash_bootmode_reset(uint32_t addr, size_t size){
	return flash_zero(addr, size);
}
