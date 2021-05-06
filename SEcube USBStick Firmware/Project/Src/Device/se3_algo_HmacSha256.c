/**
  ******************************************************************************
  * File Name          : se3_algo_HmacSha256.c
  * Description        : HMAC-SHA-256 primitives/crypto handlers
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

#include "se3_algo_HmacSha256.h"


uint16_t se3_algo_HmacSha256_init(se3_flash_key* key, uint16_t mode, uint8_t* ctx)
{
	B5_tHmacSha256Ctx* sha_hmac = (B5_tHmacSha256Ctx *) ctx;

	if ((int32_t)key->data_size <= 0) { // checking as int32_t because B5_HmacSha256_Init() takes int32_t as param
		return (SE3_ERR_PARAMS);
	}
	if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Init(sha_hmac, key->data, key->data_size)) {
		SE3_TRACE(("[algo_sha256hmac_init.init] B5_HmacSha256_Init failed\n"));
		return (SE3_ERR_PARAMS);
	}

	return (SE3_OK);
}


uint16_t se3_algo_HmacSha256_update(
	uint8_t* ctx, uint16_t flags,
	uint16_t datain1_len, const uint8_t* datain1,
	uint16_t datain2_len, const uint8_t* datain2,
	uint16_t* dataout_len, uint8_t* dataout)
{

	B5_tHmacSha256Ctx* sha_hmac = (B5_tHmacSha256Ctx *)ctx;

	bool do_update = (datain1_len > 0);
	bool do_finit = (flags & SE3_CRYPTO_FLAG_FINIT);

	if (do_update) {
		// update
		if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Update(sha_hmac, datain1, datain1_len)) {
			SE3_TRACE(("[algo_sha256hmac_init.update] B5_HmacSha256_Update failed\n"));
			return SE3_ERR_HW;
		}
	}

	if (do_finit) {
		if (B5_HMAC_SHA256_RES_OK != B5_HmacSha256_Finit(sha_hmac, dataout)) {
			SE3_TRACE(("[algo_sha256hmac_init.update] B5_HmacSha256_Finit failed\n"));
			return SE3_ERR_HW;
		}
		*dataout_len = 32;
	}

	return(SE3_OK);
}
