/**
  ******************************************************************************
  * File Name          : aes256.h
  * Description        : This file includes the definition of return values,
  *                      constant parameters, and public functions used to
  *                      implement AES algorithms.
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

/** \defgroup aesReturn AES return values
 * @{
 */
/** \name AES return values */
///@{
#define B5_AES256_RES_OK                                    ( 0)
#define B5_AES256_RES_INVALID_CONTEXT                       (-1)
#define B5_AES256_RES_CANNOT_ALLOCATE_CONTEXT               (-2)
#define B5_AES256_RES_INVALID_KEY_SIZE                      (-3)
#define B5_AES256_RES_INVALID_ARGUMENT                      (-4)
#define B5_AES256_RES_INVALID_MODE                          (-5)
///@}
/** @} */

/** \defgroup aesKeys AES Key, IV, Block Sizes
 * @{
 */
/** \name AES Key, IV, Block Sizes */
///@{
#define B5_AES_256              32  /**< Key Size in Bytes. */
#define B5_AES_192              24  /**< Key Size in Bytes. */
#define B5_AES_128              16  /**< Key Size in Bytes. */
#define B5_AES_IV_SIZE          16  /**< IV Size in Bytes. */
#define B5_AES_BLK_SIZE         16  /**< Block Size in Bytes. */
///@}
/** @} */

/** \defgroup aesModes AES modes
 * @{
 */
/** \name AES modes */
///@{
#define B5_AES256_OFB           1       /**< OFB full feedback encryption-decryption */
#define B5_AES256_ECB_ENC       2       /**< ECB encryption */
#define B5_AES256_ECB_DEC       3       /**< ECB decryption */
#define B5_AES256_CBC_ENC       4       /**< CBC encryption */
#define B5_AES256_CBC_DEC       5       /**< CBC decryption */
#define B5_AES256_CFB_ENC       6       /**< CFB decryption */
#define B5_AES256_CFB_DEC       7       /**< CFB decryption */
#define B5_AES256_CTR           8       /**< CTR counter mode encryption-decryption */
///@}
/** @} */

/** \defgroup aesStr AES data structures
 * @{
 */
/** \name AES data structures */
///@{
typedef struct {
    uint32_t rk[4*(14 + 1)];       /**< Precomputed round keys */
    uint8_t  Nr;                   /**< Number of rounds */
    uint8_t  InitVector[16];       /**< IV for OFB, CBC, CTR */
    uint8_t  mode;                 /**< Active mode */
    uint32_t const *Te0;
    uint32_t const *Te1;
    uint32_t const *Te2;
    uint32_t const *Te3;
    uint32_t const *Te4;
    uint32_t const *Td0;
    uint32_t const *Td1;
    uint32_t const *Td2;
    uint32_t const *Td3;
    uint32_t const *Td4;
} B5_tAesCtx;
///@}
/** @} */

/** \defgroup aesFunc AES functions
 * @{
 */
/** \name AES functions */
///@{

/**
 *
 * @brief Initialize the AES context.
 * @param ctx Pointer to the AES data structure to be initialized.
 * @param Key Pointer to the Key that must be used for encryption/decryption.
 * @param keySize Key size. See \ref aesKeys for supported sizes.
 * @param aesMode AES mode. See \ref aesModes for supported modes.
 * @return See \ref aesReturn .
 */
int32_t    B5_Aes256_Init   (B5_tAesCtx *ctx, const uint8_t *Key, int16_t keySize, uint8_t aesMode);

/**
 *
 * @brief Set the IV for the current AES context.
 * @param ctx Pointer to the AES data structure to be initialized.
 * @param IV Pointer to the IV.
 * @return See \ref aesReturn .
 */
int32_t    B5_Aes256_SetIV  (B5_tAesCtx *ctx, const uint8_t *IV);

/**
 *
 * @brief Encrypt/Decrypt data based on the status of current AES context.
 * @param ctx Pointer to the current AES context.
 * @param encData Encrypted data.
 * @param clrData Clear data.
 * @param nBlk Number of AES blocks to process.
 * @return See \ref aesReturn .
 */
int32_t    B5_Aes256_Update (B5_tAesCtx *ctx, uint8_t *encData, uint8_t *clrData, int16_t nBlk);

/**
 *
 * @brief De-initialize the current AES context.
 * @param ctx Pointer to the AES context to de-initialize.
 * @return See \ref aesReturn .
 */
int32_t    B5_Aes256_Finit  (B5_tAesCtx *ctx);
    
///@}
/** @} */

/** \defgroup cmacaesKeys CMAC-AES Key, Blk Sizes
 * @{
 */
/** \name CMAC-AES Key, Block Sizes */
///@{
#define B5_CMAC_AES_256             32  /**< Key Size in Bytes */
#define B5_CMAC_AES_192             24  /**< Key Size in Bytes */
#define B5_CMAC_AES_128             16  /**< Key Size in Bytes */
#define B5_CMAC_AES_BLK_SIZE        16  /**< Block Size in Bytes */
///@}
/** @} */

/** \defgroup cmacaesReturn CMAC-AES return values
 * @{
 */
/** \name CMAC-AES return values */
///@{
#define B5_CMAC_AES256_RES_OK                                   ( 0)
#define B5_CMAC_AES256_RES_INVALID_CONTEXT                      (-1)
#define B5_CMAC_AES256_RES_CANNOT_ALLOCATE_CONTEXT              (-2)
#define B5_CMAC_AES256_RES_INVALID_KEY_SIZE                     (-3)
#define B5_CMAC_AES256_RES_INVALID_ARGUMENT                     (-4)
///@}
/** @} */

/** \defgroup cmacaesStr CMAC-AES data structures
 * @{
 */
/** \name CMAC-AES data structures */
///@{
typedef struct {
    B5_tAesCtx  aesCtx;
    
    uint8_t        K1[32];
    uint8_t        K2[32];
    
    uint8_t        tmpBlk[B5_AES_BLK_SIZE];
    uint8_t        tmpBlkLen;
    
    uint8_t        C[B5_AES_BLK_SIZE];
} B5_tCmacAesCtx;
///@}
/** @} */

/** \defgroup cmacaesFunc CMAC-AES functions
 * @{
 */
/** \name CMAC-AES functions */
///@{
/**
 *
 * @brief Initialize the CMAC-AES context.
 * @param ctx Pointer to the CMAC-AES data structure to be initialized.
 * @param Key Pointer to the Key that must be used.
 * @param keySize Key size. See \ref cmacaesKeys for supported sizes.
 * @return See \ref cmacaesReturn .
 */
int32_t    B5_CmacAes256_Init (B5_tCmacAesCtx *ctx, const uint8_t *Key, int16_t keySize);

/**
 *
 * @brief Compute the CMAC-AES algorithm on input data depending on the current status of the CMAC-AES context.
 * @param ctx Pointer to the current CMAC-AES context.
 * @param data Pointer to the input data.
 * @param dataLen Bytes to be processed.
 * @return See \ref cmacaesReturn .
 */
int32_t    B5_CmacAes256_Update (B5_tCmacAesCtx *ctx, const uint8_t *data, int32_t dataLen);

/**
 *
 * @brief De-initialize the current CMAC-AES context.
 * @param ctx Pointer to the CMAC-AES context to de-initialize.
 * @param rSignature Pointer to a blank memory area that can store the computed output signature.
 * @return See \ref cmacaesReturn .
 */
int32_t    B5_CmacAes256_Finit (B5_tCmacAesCtx *ctx, uint8_t *rSignature);

/**
 *
 * @brief Reset the current CMAC-AES context.
 * @param ctx Pointer to the CMAC-AES context to reset.
 * @return See \ref cmacaesReturn .
 */
int32_t    B5_CmacAes256_Reset (B5_tCmacAesCtx *ctx);

/**
 *
 * @brief Compute the signature through the CMAC-AES algorithm.
 * @param data Pointer to the input data.
 * @param dataLen Input data length (in Bytes).
 * @param Key Pointer to the Key that must be used.
 * @param keySize Key size. See \ref cmacaesKeys for supported sizes.
 * @param rSignature Pointer to a blank memory area that can store the computed output signature.
 * @return See \ref cmacaesReturn .
 */
int32_t    B5_CmacAes256_Sign (const uint8_t *data, int32_t dataLen, const uint8_t *Key, int16_t keySize, uint8_t *rSignature);

///@}
/** @} */
    
#ifdef __cplusplus
}
#endif
