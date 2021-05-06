/**
  ******************************************************************************
  * File Name          : sha256.h
  * Description        : SHA-256 implementation
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

#pragma once

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup shaReturn SHA256 return values
 * @{
 */
/** \name SHA256 return values */
///@{
#define B5_SHA256_RES_OK				 						( 0)
#define B5_SHA256_RES_INVALID_CONTEXT							(-1)
#define B5_SHA256_RES_CANNOT_ALLOCATE_CONTEXT                   (-2)
#define B5_SHA256_RES_INVALID_ARGUMENT                          (-3)
///@}
/** @} */

/** \defgroup shaSize SHA256 digest and block sizes
 * @{
 */
/** \name SHA256 digest and block sizes */
///@{
#define B5_SHA256_DIGEST_SIZE       32
#define B5_SHA256_BLOCK_SIZE 		64
///@}
/** @} */

/** \defgroup shaStr SHA256 data structures
 * @{
 */
/** \name SHA256 data structures */
///@{
typedef struct
{
    uint32_t   total[2];
    uint32_t   state[8];
    uint8_t    buffer[64];    
    uint32_t   W[64];    
} B5_tSha256Ctx;
///@}
/** @} */

/** \defgroup shaFunc SHA256 functions
 * @{
 */
/** \name SHA256 functions */
///@{

/**
 * @brief Initialize the SHA256 context.
 * @param ctx Pointer to the SHA256 data structure to be initialized.
 * @return See \ref shaReturn .
 */
int32_t B5_Sha256_Init (B5_tSha256Ctx *ctx);

/**
 * @brief Compute the SHA256 algorithm on input data depending on the current status of the SHA256 context.
 * @param ctx Pointer to the current SHA context.
 * @param data Pointer to the input data.
 * @param dataLen Bytes to be processed.
 * @return See \ref shaReturn .
 */
int32_t B5_Sha256_Update (B5_tSha256Ctx *ctx, const uint8_t *data, int32_t dataLen);

/**
 * @brief De-initialize the current SHA256 context.
 * @param ctx Pointer to the SHA context to de-initialize.
 * @param rDigest Pointer to a blank memory area that can store the computed output digest.
 * @return See \ref shaReturn .
 */
int32_t B5_Sha256_Finit (B5_tSha256Ctx *ctx, uint8_t *rDigest);
///@}
/** @} */

/** \defgroup hmacshaReturn HMAC-SHA256 return values
 * @{
 */
/** \name SHA256 return values */
///@{
#define B5_HMAC_SHA256_RES_OK				 					( 0)
#define B5_HMAC_SHA256_RES_INVALID_CONTEXT						(-1)
#define B5_HMAC_SHA256_RES_CANNOT_ALLOCATE_CONTEXT              (-2)
#define B5_HMAC_SHA256_RES_INVALID_ARGUMENT						(-3)
///@}
/** @} */

/** \defgroup hmacshaStr HMAC-SHA256 data structures
 * @{
 */
/** \name SHA256 data structures */
///@{
typedef struct
{
   B5_tSha256Ctx        shaCtx;
   uint8_t     		iPad[64];
   uint8_t     		oPad[64];
} B5_tHmacSha256Ctx;
///@}
/** @} */

/** \defgroup hmacshaFunc HMAC-SHA256 functions
 * @{
 */
/** \name HMAC-SHA256 functions */
///@{

/**
 * @brief Initialize the HMAC-SHA256 context.
 * @param ctx Pointer to the HMAC-SHA256 data structure to be initialized.
 * @param Key Pointer to the Key that must be used.
 * @param keySize Key size.
 * @return See \ref hmacshaReturn .
 */
int32_t B5_HmacSha256_Init (B5_tHmacSha256Ctx *ctx, const uint8_t *Key, int16_t keySize);

/**
 * @brief Compute the HMAC-SHA256 algorithm on input data depending on the current status of the HMAC-SHA256 context.
 * @param ctx Pointer to the current HMAC-SHA256 context.
 * @param data Pointer to the input data.
 * @param dataLen Bytes to be processed.
 * @return See \ref hmacshaReturn .
 */
int32_t B5_HmacSha256_Update (B5_tHmacSha256Ctx *ctx, const uint8_t *data, int32_t dataLen);

/**
 * @brief De-initialize the current HMAC-SHA256 context.
 * @param ctx Pointer to the HMAC-SHA256 context to de-initialize.
 * @param rDigest Pointer to a blank memory area that can store the computed output digest.
 * @return See \ref hmacshaReturn .
 */
int32_t B5_HmacSha256_Finit (B5_tHmacSha256Ctx *ctx, uint8_t *rDigest);
///@}
/** @} */

#ifdef __cplusplus
}
#endif
