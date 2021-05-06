/**
  ******************************************************************************
  * File Name          : se3_algo_Aes.c
  * Description        : AES primitives/crypto handlers
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

#include "se3_algo_Aes.h"

uint16_t se3_algo_Aes_init(se3_flash_key* key, uint16_t mode, uint8_t* ctx){
    B5_tAesCtx* aes = (B5_tAesCtx*)ctx;
	uint16_t feedback = mode & 0x07;
	uint16_t direction = (mode & SE3_DIR_ENCRYPT) ? SE3_DIR_ENCRYPT : SE3_DIR_DECRYPT;
	uint8_t b5_mode = 0;

	switch (key->data_size) {
		case B5_AES_256:
			break;
		case B5_AES_192:
			break;
		case B5_AES_128:
			break;
		default: // unsupported key size
			return SE3_ERR_PARAMS;
	}

	switch (direction) {
		case (SE3_DIR_ENCRYPT):
			switch (feedback) {
				case SE3_FEEDBACK_ECB:
					b5_mode = B5_AES256_ECB_ENC;
					break;
				case SE3_FEEDBACK_CBC:
					b5_mode = B5_AES256_CBC_ENC;
					break;
				case SE3_FEEDBACK_CTR:
					b5_mode = B5_AES256_CTR;
					break;
				case SE3_FEEDBACK_OFB:
					b5_mode = B5_AES256_OFB;
					break;
				case SE3_FEEDBACK_CFB:
					b5_mode = B5_AES256_CFB_ENC;
					break;
				default:
					return SE3_ERR_PARAMS;
			}
			break;
		case (SE3_DIR_DECRYPT):
			switch (feedback) {
				case SE3_FEEDBACK_ECB:
					b5_mode = B5_AES256_ECB_DEC;
					break;
				case SE3_FEEDBACK_CBC:
					b5_mode = B5_AES256_CBC_DEC;
					break;
				case SE3_FEEDBACK_CTR:
					b5_mode = B5_AES256_CTR;
					break;
				case SE3_FEEDBACK_OFB:
					b5_mode = B5_AES256_OFB;
					break;
				case SE3_FEEDBACK_CFB:
					b5_mode = B5_AES256_CFB_DEC;
					break;
				default:
					return SE3_ERR_PARAMS;
			}
			break;
		default:
			return SE3_ERR_PARAMS;
	}

    if (B5_AES256_RES_OK != B5_Aes256_Init(aes, key->data, (int16_t)key->data_size, b5_mode)) {
        SE3_TRACE(("[algo_aes256.init] B5_Aes256_Init failed\n"));
        return SE3_ERR_PARAMS;
    }

    return SE3_OK;
}

uint16_t se3_algo_Aes_update(
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
	

	do_setiv = flags & SE3_CRYPTO_FLAG_SETIV;
	do_update = datain2_len > 0;
	do_finit = flags & SE3_CRYPTO_FLAG_FINIT;

    // check params
	if (do_setiv && (datain1_len != B5_AES_BLK_SIZE)) {
		SE3_TRACE(("[algo_aes256.update] invalid IV size\n"));
		return SE3_ERR_PARAMS;
	}
    if (do_update) {
        if (datain2_len % B5_AES_BLK_SIZE != 0) {
            SE3_TRACE(("[algo_aes256.update] data size not a multiple of block size\n"));
            return SE3_ERR_PARAMS;
        }
    }


    if (do_setiv) {
        // set IV
        if (B5_AES256_RES_OK != B5_Aes256_SetIV(aes, datain1)) {
            SE3_TRACE(("[algo_aes256.update] B5_Aes256_SetIV failed\n"));
            return SE3_ERR_HW;
        }
    }

    if (do_update) { // update
		nblocks = datain2_len / B5_AES_BLK_SIZE;

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
		case B5_AES256_OFB:
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
    }

    if (do_finit) {
        if (B5_AES256_RES_OK != B5_Aes256_Finit(aes)) {
            SE3_TRACE(("[algo_aes256.update] B5_Aes256_Finit failed\n"));
            return SE3_ERR_HW;
        }
    }

    return SE3_OK;
}
