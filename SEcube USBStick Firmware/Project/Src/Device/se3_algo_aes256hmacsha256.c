/**
  ******************************************************************************
  * File Name          : se3_algo_aes256hmacsha256.c
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

#include "se3_algo_aes256hmacsha256.h"

// TODO remove and use AesHmacSha256s

const uint8_t salt[PBKDF2_SALT_LEN] = {
	0x1f, 0x25, 0x6c, 0x72, 0xa1, 0x55, 0x9d, 0xbc,
	0x30, 0x0a, 0x22, 0x4f, 0x2a, 0x10, 0xa5, 0xfa,
	0x83, 0x41, 0xc9, 0x62, 0x7b, 0x08, 0xab, 0x65,
	0x7f, 0x33, 0x21, 0x3c, 0xc6, 0x5a, 0x37, 0xfc
};

uint16_t se3_algo_aes256hmacsha256_init(
    se3_flash_key* key, uint16_t mode, uint8_t* ctx)
{
    B5_tAesCtx* aes = (B5_tAesCtx*)ctx;
	
	uint8_t feedback = mode & 0x07;
	uint16_t direction = (mode & SE3_DIR_ENCRYPT) ? SE3_DIR_ENCRYPT : SE3_DIR_DECRYPT;
	uint8_t b5_mode = 0;
	/*HMAC Authentication support available*/
	B5_tHmacSha256Ctx *hmacCtx = (B5_tHmacSha256Ctx*)(ctx+sizeof(B5_tAesCtx));
	//uint8_t *k1 = (uint8_t *)(ctx + sizeof(B5_tAesCtx) + sizeof(B5_tHmacSha256Ctx));
	//uint8_t *do_pbkdf2 = (uint8_t *)(ctx + sizeof(B5_tAesCtx) + sizeof(B5_tHmacSha256Ctx) + 32);
	uint8_t derivated_key[PBKDF2_OUTPUT_KEY_LEN];
	switch (direction) {
	case (SE3_DIR_ENCRYPT):
		switch (feedback) {
		case SE3_FEEDBACK_ECB: b5_mode = B5_AES256_ECB_ENC; break;
		case SE3_FEEDBACK_CBC: b5_mode = B5_AES256_CBC_ENC; break;
		case SE3_FEEDBACK_CTR: b5_mode = B5_AES256_CTR; break;
		case SE3_FEEDBACK_OFB: b5_mode = B5_AES256_OFB; break;
		default: return SE3_ERR_PARAMS;
		}
		break;
	case (SE3_DIR_DECRYPT):
		switch (feedback) {
		case SE3_FEEDBACK_ECB: b5_mode = B5_AES256_ECB_DEC; break;
		case SE3_FEEDBACK_CBC: b5_mode = B5_AES256_CBC_DEC; break;
		case SE3_FEEDBACK_CTR: b5_mode = B5_AES256_CTR; break;
		case SE3_FEEDBACK_OFB: b5_mode = B5_AES256_OFB; break;
		default: return SE3_ERR_PARAMS;
		}
		break;
	default:
		return SE3_ERR_PARAMS;
	}
	PBKDF2HmacSha256(key->data, key->data_size, salt, PBKDF2_SALT_LEN, PBKDF2_ITERATION, derivated_key, PBKDF2_OUTPUT_KEY_LEN);
	if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Init(hmacCtx, derivated_key + PBKDF2_INPUT_KEY_LEN, PBKDF2_INPUT_KEY_LEN)){
		SE3_TRACE(("[algo_aes256.init] B5_HmacSha256_Init failed\n"));
		return SE3_ERR_PARAMS;
	}
	if (B5_AES256_RES_OK != B5_Aes256_Init(aes, key->data, key->data_size, b5_mode)) {
		SE3_TRACE(("[algo_aes256.init] B5_Aes256_Init failed\n"));
		return SE3_ERR_PARAMS;
	}
    return SE3_OK;
}

uint16_t se3_algo_aes256hmacsha256_update(
    uint8_t* ctx, uint16_t flags,
    uint16_t datain1_len, const uint8_t* datain1,
    uint16_t datain2_len, const uint8_t* datain2,
    uint16_t* dataout_len, uint8_t* dataout)
{
    B5_tAesCtx* aes = (B5_tAesCtx*)ctx;
    size_t nblocks = 0;
    uint8_t* data_enc, *data_dec;
    bool do_setiv = false;
    bool do_update = false;
    bool do_finit = false;
	/*If we want to add also HMAC digest when encrypting modify next lines*/
	B5_tHmacSha256Ctx *hmacCtx = (B5_tHmacSha256Ctx*)(ctx+sizeof(B5_tAesCtx));
	uint8_t old_auth[32], *data_auth;
	uint16_t direction = (flags & SE3_DIR_ENCRYPT) ? SE3_DIR_ENCRYPT : SE3_DIR_DECRYPT;
	

    // check params
    if (datain1_len > 0) {
        if (datain1_len != B5_AES_BLK_SIZE) {
            SE3_TRACE(("[algo_aes256.update] invalid IV size\n"));
            return SE3_ERR_PARAMS;
        }
        do_setiv = true;
    }
	
    if (datain2_len > 0) {
        if (datain2_len % B5_AES_BLK_SIZE != 0) {
            SE3_TRACE(("[algo_aes256.update] data size not a multiple of block size\n"));
            return SE3_ERR_PARAMS;
        }
        do_update = true;
        nblocks = datain2_len / B5_AES_BLK_SIZE;
		if (datain2_len > SE3_CRYPTO_MAX_DATAOUT - 32){
			SE3_TRACE(("[algo_aes256.update] data size too small\n"));
			return SE3_ERR_PARAMS;
		}
    }
    if (flags & SE3_CRYPTO_FLAG_FINIT) {
        do_finit = true;
    }
	
	
	
    if (do_setiv) {
        // set IV

        if (B5_AES256_RES_OK != B5_Aes256_SetIV(aes, datain1)) {
            SE3_TRACE(("[algo_aes256.update] B5_Aes256_SetIV failed\n"));
            return SE3_ERR_HW;
        }
    }

    if (do_update) {
        // update

        switch (aes->mode) {
        case B5_AES256_ECB_DEC:
        case B5_AES256_CBC_DEC:
        case B5_AES256_CFB_DEC:
            data_enc = (uint8_t*)datain2;
            data_dec = dataout;
            break;
        case B5_AES256_ECB_ENC:
        case B5_AES256_CBC_ENC:
        case B5_AES256_CFB_ENC:
        case B5_AES256_CTR:
        default:
            data_enc = dataout;
            data_dec = (uint8_t*)datain2;
            break;
        }

        if (B5_AES256_RES_OK != B5_Aes256_Update(aes, data_enc, data_dec, (int16_t)nblocks)) {
            SE3_TRACE(("[algo_aes256.update] B5_Aes256_Update failed\n"));
            return SE3_ERR_HW;
        }
        *dataout_len = datain2_len;
		
		if (direction == SE3_DIR_ENCRYPT) data_auth = dataout;
			else data_auth = (uint8_t*)datain2;

		if(B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Update(hmacCtx, data_auth, datain2_len)){
			SE3_TRACE(("[algo_aes256.update] B5_HmacSha256_Update failed\n"));
			return SE3_ERR_HW;
		}

    }

    if (do_finit) {
        if (B5_AES256_RES_OK != B5_Aes256_Finit(aes)) {
            SE3_TRACE(("[algo_aes256.update] B5_Aes256_Finit failed\n"));
            return SE3_ERR_HW;
        }
		if (direction == SE3_DIR_ENCRYPT){
			if(B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Finit(hmacCtx, data_enc+datain2_len)){
				SE3_TRACE(("[algo_aes256.update] B5_HmacSha256_Finit failed\n"));
				return SE3_ERR_HW;
			}
		} else {
			if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Finit(hmacCtx, old_auth)){
				SE3_TRACE(("[algo_aes256.update] B5_HmacSha256_Finit failed\n"));
				return SE3_ERR_HW;
			}
			if (!memcmp(old_auth, (data_enc + datain2_len), 32)){
				SE3_TRACE(("[algo_aes256.update] B5_HmacSha256_Finit NOT AUTHENTICATED\n"));
				return SE3_ERR_AUTH;
			}
		}		
    }

    return SE3_OK;
}
