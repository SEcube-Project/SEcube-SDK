/**
  ******************************************************************************
  * File Name          : se3_algo_aes256hmacsha256.h
  * Description        : AES-HMAC-SHA-256 primitives
  ******************************************************************************
  *
  * Copyright � 2016-present Blu5 Group <https://www.blu5group.com>
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
#include "se3_security_core.h"
#include "pbkdf2.h"

#define PBKDF2_SALT_LEN	32
#define PBKDF2_ITERATION 1000
#define PBKDF2_INPUT_KEY_LEN 32
#define PBKDF2_OUTPUT_KEY_LEN 64

uint16_t se3_algo_aes256hmacsha256_init(
    se3_flash_key* key, uint16_t mode, uint8_t* ctx);

uint16_t se3_algo_aes256hmacsha256_update(
    uint8_t* ctx, uint16_t flags,
    uint16_t datain1_len, const uint8_t* datain1,
    uint16_t datain2_len, const uint8_t* datain2,
    uint16_t* dataout_len, uint8_t* dataout);
