/**
  ******************************************************************************
  * File Name          : se3_rand.c
  * Description        : SEcube TRNG
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

#include "se3_rand.h"
#include "rng.h"
#include "stm32f4xx_hal_rng.h"


bool se3_rand32(uint32_t *val){
	size_t i;
	HAL_StatusTypeDef ret;
	for(i=0; i<20; i++){
		ret=HAL_RNG_GenerateRandomNumber(&hrng, val);
		if(ret==HAL_OK){
			return true;
		}
		else if(ret==HAL_ERROR){
			return false;
		}
	}
	return false;
}

uint16_t se3_rand(uint16_t size, uint8_t* data){
	uint32_t tmp;
	size_t i,n;

	n=size/4;
	for(i=0;i<n;i++){
		if(!se3_rand32((uint32_t*)data)){
			return 0;
		}
		data+=4;
	}
	n=size%4;
	if(n!=0){
		if(!se3_rand32(&tmp)){
			return 0;
		}
		for(i=0;i<n;i++){
			data[i]=((uint8_t*)&tmp)[i];
		}
	}
	return size;
}
