/**
  ******************************************************************************
  * File Name          : se3_algo_AesHmacSha256s.c
  * Description        : AES-HMAC-SHA-256 primitives
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
*  \file se3_algo_AesHmacSha256s.c
*  \brief SE3_ALGO_AES_HMACSHA256 crypto handlers
*/

#include "se3_algo_AesHmacSha256s.h"

enum {
	SE3_ALGO_STATE_KEYS_NOT_INITIALIZED = 0,
	SE3_ALGO_STATE_KEYS_INITIALIZED = 1,
};

typedef struct {
	B5_tAesCtx* aes;
	B5_tHmacSha256Ctx* hmac;
	uint8_t* keys;
	uint16_t* key_size;
	uint8_t* state;
	uint8_t* mode;
	uint8_t* direction;
} AesHmacSha256s_ctx;



uint16_t se3_algo_AesHmacSha256s_init(se3_flash_key* key, uint16_t mode, uint8_t* ctx){
	AesHmacSha256s_ctx myctx;
	uint8_t* p = ctx;

	uint16_t feedback = mode & 0x07;
	uint16_t direction = (mode & SE3_DIR_ENCRYPT) ? SE3_DIR_ENCRYPT : SE3_DIR_DECRYPT;

	myctx.aes = (B5_tAesCtx*)p; p += sizeof(B5_tAesCtx);
	myctx.hmac = (B5_tHmacSha256Ctx*)p; p += sizeof(B5_tHmacSha256Ctx);
	myctx.keys = p; p += 64;
	myctx.key_size = (uint16_t*)p; p += sizeof(uint16_t);
	myctx.mode = (uint8_t*)p; p += sizeof(uint8_t);
	myctx.state = (uint8_t*)p; p += sizeof(uint8_t);
	myctx.direction = (uint8_t*)p; p += sizeof(uint8_t);

	switch (key->data_size) {
	case B5_AES_256:
		break;
	case B5_AES_192:
		break;
	case B5_AES_128:
		break;
	default:
		return SE3_ERR_PARAMS; // unsupported key size
	}

	*myctx.direction = (direction == SE3_DIR_ENCRYPT) ? (0) : (1);

	switch (direction) {
	case (SE3_DIR_ENCRYPT):
		switch (feedback) {
		case SE3_FEEDBACK_ECB: *(myctx.mode) = B5_AES256_ECB_ENC; break;
		case SE3_FEEDBACK_CBC: *(myctx.mode) = B5_AES256_CBC_ENC; break;
		case SE3_FEEDBACK_CTR: *(myctx.mode) = B5_AES256_CTR; break;
		case SE3_FEEDBACK_OFB: *(myctx.mode) = B5_AES256_OFB; break;
		case SE3_FEEDBACK_CFB: *(myctx.mode) = B5_AES256_CFB_ENC; break;
		default: return SE3_ERR_PARAMS;
		}
		break;
	case (SE3_DIR_DECRYPT):
		switch (feedback) {
		case SE3_FEEDBACK_ECB: *(myctx.mode) = B5_AES256_ECB_DEC; break;
		case SE3_FEEDBACK_CBC: *(myctx.mode) = B5_AES256_CBC_DEC; break;
		case SE3_FEEDBACK_CTR: *(myctx.mode) = B5_AES256_CTR; break;
		case SE3_FEEDBACK_OFB: *(myctx.mode) = B5_AES256_OFB; break;
		case SE3_FEEDBACK_CFB: *(myctx.mode) = B5_AES256_CFB_DEC; break;
		default: return SE3_ERR_PARAMS;
		}
		break;
	default:
		return SE3_ERR_PARAMS;
	}

	*(myctx.state) = SE3_ALGO_STATE_KEYS_NOT_INITIALIZED;
	memcpy(myctx.keys, key->data, key->data_size);
	*(myctx.key_size) = key->data_size;

	return SE3_OK;
}

uint16_t se3_algo_AesHmacSha256s_update(
	uint8_t* ctx, uint16_t flags,
	uint16_t datain1_len, const uint8_t* datain1,
	uint16_t datain2_len, const uint8_t* datain2,
	uint16_t* dataout_len, uint8_t* dataout)
{
	AesHmacSha256s_ctx myctx;
	uint8_t* p = ctx;

	size_t nblocks = 0;
	size_t outsize = 0;
	uint8_t* pdataout = dataout;

	bool do_setnonce = (flags & SE3_CRYPTO_FLAG_SETNONCE);
	bool do_update = (datain2_len > 0);
	bool do_finit = (flags & SE3_CRYPTO_FLAG_FINIT);
	bool do_auth = (flags & SE3_CRYPTO_FLAG_AUTH);
	bool do_reset = (flags & SE3_CRYPTO_FLAG_RESET);

	myctx.aes = (B5_tAesCtx*)p; p += sizeof(B5_tAesCtx);
	myctx.hmac = (B5_tHmacSha256Ctx*)p; p += sizeof(B5_tHmacSha256Ctx);
	myctx.keys = p; p += 64;
	myctx.key_size = (uint16_t*)p; p += sizeof(uint16_t);
	myctx.mode = (uint8_t*)p; p += sizeof(uint8_t);
	myctx.state = (uint8_t*)p; p += sizeof(uint8_t);
	myctx.direction = (uint8_t*)p; p += sizeof(uint8_t);


	// check params
	if (do_setnonce) {
		if (*myctx.state == SE3_ALGO_STATE_KEYS_INITIALIZED){
			return SE3_ERR_STATE;
		}
		if (do_reset || do_auth || do_update) {
			return SE3_ERR_PARAMS;
		}
		if (do_finit){
			return SE3_OK;
		}
		// derive keys
		PBKDF2HmacSha256(myctx.keys, *(myctx.key_size), datain1, datain1_len, 1, myctx.keys, 2 * (*(myctx.key_size)));
		if (B5_AES256_RES_OK != B5_Aes256_Init(myctx.aes, myctx.keys, *(myctx.key_size), *(myctx.mode))) {
			return SE3_ERR_PARAMS;
		}
		if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Init(myctx.hmac, myctx.keys + (*(myctx.key_size)), *(myctx.key_size))) {
			return SE3_ERR_PARAMS;
		}
		*(myctx.state) = SE3_ALGO_STATE_KEYS_INITIALIZED;
	}
	else {
		if (do_reset && (datain1_len != B5_AES_BLK_SIZE && datain1_len != 0)) {
			SE3_TRACE(("[algo_aes256.update] invalid IV size\n"));
			return SE3_ERR_PARAMS;
		}
		if (do_update && ((datain2_len % B5_AES_BLK_SIZE) != 0)) {
			return SE3_ERR_PARAMS;
		}

		if (*(myctx.state) == SE3_ALGO_STATE_KEYS_NOT_INITIALIZED) {
			// derive keys with empty salt
			PBKDF2HmacSha256(myctx.keys, *(myctx.key_size), NULL, 0, 1, myctx.keys, 2 * (*(myctx.key_size)));
			if (B5_AES256_RES_OK != B5_Aes256_Init(myctx.aes, myctx.keys, *(myctx.key_size), *(myctx.mode))) {
				return SE3_ERR_PARAMS;
			}
			if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Init(myctx.hmac, myctx.keys + (*(myctx.key_size)), *(myctx.key_size))) {
				return SE3_ERR_PARAMS;
			}
			*(myctx.state) = SE3_ALGO_STATE_KEYS_INITIALIZED;
		}

		// compute output size
		outsize = 0;
		if (do_update){
			outsize += datain2_len;
		}
		if (do_auth){
			outsize += B5_SHA256_DIGEST_SIZE;
		}

		if (outsize > SE3_CRYPTO_MAX_DATAOUT) {
			return SE3_ERR_PARAMS;
		}

		if (do_reset) {
			if (datain1_len > 0) {
				B5_Aes256_SetIV(myctx.aes, datain1);
			}
			B5_HmacSha256_Init(myctx.hmac, myctx.keys + (*(myctx.key_size)), *(myctx.key_size));
			if (datain1_len > 0) {
				B5_HmacSha256_Update(myctx.hmac, datain1, datain1_len);
			}
		}

		if (do_update) {
			nblocks = datain2_len / B5_AES_BLK_SIZE;

			if (*myctx.direction == 1) { // decrypt
				if (B5_AES256_RES_OK != B5_HmacSha256_Update(myctx.hmac, datain2, datain2_len)) {
					return SE3_ERR_HW;
				}
				switch (*myctx.mode)
				{
				case B5_AES256_CTR:
				case B5_AES256_OFB:
					if (B5_HMAC_SHA256_RES_OK != B5_Aes256_Update(myctx.aes, pdataout, (uint8_t*)datain2, (uint16_t)nblocks)) {
						return SE3_ERR_HW;
					}
					break;
				default:
					if (B5_HMAC_SHA256_RES_OK != B5_Aes256_Update(myctx.aes, (uint8_t*)datain2, pdataout, (uint16_t)nblocks)) {
						return SE3_ERR_HW;
					}
				}

			}
			else { // encrypt
				if (B5_AES256_RES_OK != B5_Aes256_Update(myctx.aes, pdataout, (uint8_t*)datain2, (uint16_t)nblocks)) {
					return SE3_ERR_HW;
				}
				if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Update(myctx.hmac, pdataout, datain2_len)) {
					return SE3_ERR_HW;
				}
			}

			pdataout += datain2_len;
		}

		if (do_auth) {
			B5_HmacSha256_Finit(myctx.hmac, pdataout);
			pdataout += B5_SHA256_DIGEST_SIZE;
		}

		*dataout_len = (uint16_t)outsize;
	}

	return SE3_OK;
}
