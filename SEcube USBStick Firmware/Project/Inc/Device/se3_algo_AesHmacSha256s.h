/**
  ******************************************************************************
  * File Name          : se3_algo_AesHmacSha256s.h
  * Description        : AES-HMAC-SHA-256 primitives/crypto handlers
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

/**
 *  \file se3_algo_AesHmacSha256s.h
 *  \brief SE3_ALGO_AES_HMACSHA256 crypto handlers
 */

#pragma once
#include "se3_security_core.h"

#include "pbkdf2.h"

/** \brief SE3_ALGO_AES_HMACSHA256 init handler
 *  
 *  Supported modes
 *  Any combination of one of {SE3_DIR_ENCRYPT, SE3_DIR_DECRYPT} and one of
 *    {SE3_FEEDBACK_ECB, SE3_FEEDBACK_CBC, SE3_FEEDBACK_CFB, SE3_FEEDBACK_OFB, SE3_FEEDBACK_CTR}
 *  
 *  Supported key sizes
 *  128-bit, 192-bit, 256-bit
 */
uint16_t se3_algo_AesHmacSha256s_init(
    se3_flash_key* key, uint16_t mode, uint8_t* ctx);

/** \brief SE3_ALGO_AES_HMACSHA256 update handler
 *
 *  Supported operations
 *  SE3_CRYPTO_FLAG_SETNONCE: set nonce for AES and HMAC key derivation. Optional. If used,
 *    it must be the first operation performed after initialization. Otherwise, the keys
 *    will be derived from the master key without any salt.
 *    Cannot be combined with any other operation.
 *  (default): encrypt/decrypt datain2 and update HmacSha256 context with datain2. Not executed
 *    if datain2 is empty (zero-length)
 *  SE3_CRYPTO_FLAG_RESET: set new IV from datain1 and reset the HmacSha256 context, also
 *    authenticating the IV.
 *    If the IV is empty (zero-length), no IV will be set, and the HmacSha256 will be reset.
 *  SE3_CRYPTO_FLAG_AUTH: produce authentication tag and append to dataout
 *  SE3_CRYPTO_FLAG_FINIT: release session
 *
 *  Combined operations are executed in the following order:
 *    SE3_CRYPTO_FLAG_RESET
 *    (default)
 *    SE3_CRYPTO_FLAG_AUTH
 *    SE3_CRYPTO_FLAG_FINIT
 *  
 *  Contribution of each operation to the output size:
 *    (default): + datain2_len
 *    SE3_CRYPTO_FLAG_AUTH: + B5_SHA256_DIGEST_SIZE
 *    Others: + 0
 */
uint16_t se3_algo_AesHmacSha256s_update(
    uint8_t* ctx, uint16_t flags,
    uint16_t datain1_len, const uint8_t* datain1,
    uint16_t datain2_len, const uint8_t* datain2,
    uint16_t* dataout_len, uint8_t* dataout);
