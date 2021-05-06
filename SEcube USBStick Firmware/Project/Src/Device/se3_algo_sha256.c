/**
  ******************************************************************************
  * File Name          : se3_algo_sha256.c
  * Description        : SHA-256 primitives
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

#include "se3_algo_sha256.h"


// key is not used;  mode is not used
uint16_t se3_algo_Sha256_init(se3_flash_key* key, uint16_t mode, uint8_t* ctx) {
	B5_tSha256Ctx* sha = (B5_tSha256Ctx*)ctx;
	
	if (B5_SHA256_RES_OK != B5_Sha256_Init(sha)) {
		SE3_TRACE(("[algo_sha256.init] B5_Sha256_Init failed\n"));
		return (SE3_ERR_PARAMS);
	}
	
	return (SE3_OK);
}


// datain2 is not used; datain2_len is not used
uint16_t se3_algo_Sha256_update(
	uint8_t* ctx, uint16_t flags,
	uint16_t datain1_len, const uint8_t* datain1,
	uint16_t datain2_len, const uint8_t* datain2,
	uint16_t* dataout_len, uint8_t* dataout) {

	B5_tSha256Ctx* sha = (B5_tSha256Ctx*)ctx;

	bool do_update = (datain1_len > 0);
	bool do_finit = (flags & SE3_CRYPTO_FLAG_FINIT);

	if (do_update) {
		// update
		if (B5_SHA256_RES_OK != B5_Sha256_Update(sha, datain1, datain1_len)) {
			SE3_TRACE(("[algo_sha256.update] B5_Sha256_Update failed\n"));
			return SE3_ERR_HW;
		}
	}

	if (do_finit) {
		if (B5_SHA256_RES_OK != B5_Sha256_Finit(sha, dataout)) {
			SE3_TRACE(("[algo_sha256.update] B5_Sha256_Finit failed\n"));
			return SE3_ERR_HW;
		}
		*dataout_len = 32;
	}

	return(SE3_OK);
}
