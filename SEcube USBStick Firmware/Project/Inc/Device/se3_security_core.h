/**
  ******************************************************************************
  * File Name          : se3_security_core.h
  * Description        : Low level security functionalities
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
 *  \file se3_security_core.h
 *  \brief Security core
 */
#pragma once

#include "se3_core_time.h"
#include "se3c1def.h"
#include "se3_memory.h"
#include "se3_keys.h"
#include "aes256.h"
#include "sha256.h"
#include "pbkdf2.h"

enum {
	SE3_SESSIONS_BUF = (32*1024),  ///< session buffer size
	SE3_SESSIONS_MAX = 100  ///< maximum number of sessions
};

extern enum {
	SE3_AES256,
	SE3_CRC16,
	SE3_PBKDF2,
	SE3_SHA256
}se3_crypto_algorithm;

// ---- records ----

enum {
	SE3_FLASH_TYPE_RECORD = 0xF0  ///< flash node type: record
};

enum {
	SE3_RECORD_SIZE_TYPE = 2,  ///< record.type field size
	SE3_RECORD_OFFSET_TYPE = 0, ///< record.type field offset
	SE3_RECORD_OFFSET_DATA = 2, ///< record.data field offset
};

/** \brief Record information */
typedef struct SE3_RECORD_INFO_ {
    uint16_t read_access;  ///< required access level for read
    uint16_t write_access;  ///< required access level for write
} SE3_RECORD_INFO;


typedef struct SE3_SECURITY_INFO_ {
    SE3_RECORD_INFO records[SE3_RECORD_MAX];
    se3_mem sessions;
    uint16_t sessions_algo[SE3_SESSIONS_MAX];
} SE3_SECURITY_INFO;

/** \brief crypto_init function type */
typedef uint16_t(*se3_crypto_init_handler)(
	se3_flash_key* key, uint16_t mode, uint8_t* ctx);

/** \brief crypto_update function type */
typedef uint16_t(*se3_crypto_update_handler)(
	uint8_t* ctx, uint16_t flags,
	uint16_t datain1_len, const uint8_t* datain1,
	uint16_t datain2_len, const uint8_t* datain2,
	uint16_t* dataout_len, uint8_t* dataout);

/** \brief algorithm descriptor type */
typedef struct se3_algo_descriptor_ {
	se3_crypto_init_handler init;  ///< crypto_init function
	se3_crypto_update_handler update;  ///< crypto_update function
	uint16_t size;  ///< context size size
	char display_name[16];  ///< name for the algorithm list API
	uint16_t display_type;  ///< type for the algorithm list API
	uint16_t display_block_size;  ///< block size for the algorithm list API
	uint16_t display_key_size[SE3_CMD1_CRYPTO_ALGOINFO_KEY_NUM];  /**< Supported key size. Up to 10 different sizes preallocated. */
} se3_algo_descriptor;

typedef struct se3_payload_cryptoctx_ {
	B5_tAesCtx aesenc;
    B5_tAesCtx aesdec;
	B5_tHmacSha256Ctx hmac;
	uint8_t hmac_key[B5_AES_256];
    uint8_t auth[B5_SHA256_DIGEST_SIZE];
} se3_payload_cryptoctx;

/** \brief Write record
 *
 *  Set data of a record
 *  \remark if a flash operation fails, the hwerror flag is set.
 *  \param type type of record
 *  \param data new data to be written to record
 *  \return true on success; false if the record does not exist
 */
bool record_set(uint16_t type, const uint8_t* data);

/** \brief Read record
*
*  Get data of a record.
*  \param type type of record
*  \param data output buffer
*  \return true on success; false if the record does not exist or has never been written
*/
bool record_get(uint16_t type, uint8_t* data);

// ---- crypto ----

/** \brief CRYPTO_INIT handler
 *
 *  Initialize a cryptographic context
 */
uint16_t crypto_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief CRYPTO_UPDATE handler
 *
 *  Use a cryptographic context
 */
uint16_t crypto_update(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** @brief Get list of available algorithms, with additional details. */
uint16_t crypto_list(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Security Core initialization
 *
 *  Inizialitazion of Security Core data structures
 */
void se3_security_core_init();


/** \brief Crypto algo initializator
 *
 *  Initialise the cryptographics algorithms
 */
void se3_payload_cryptoinit(se3_payload_cryptoctx* ctx, const uint8_t* key);


/** \brief data buffer encrypt function
 *
 *  encrypt the data buffer by using the algorithm
 *  assigned by SEkey described by the crypto_algo
 *  input parameter
 */
bool se3_payload_encrypt(se3_payload_cryptoctx* ctx, uint8_t* auth, uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags, uint8_t crypto_algo);


/** \brief data buffer decrypt function
 *
 *  decrypt the data buffer by using the algorithm
 *  assigned by SEkey described by the crypto_algo
 *  input parameter;
 */
bool se3_payload_decrypt(se3_payload_cryptoctx* ctx, const uint8_t* auth, const uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags, uint8_t crypto_algo);



/** \brief globals */
extern SE3_SECURITY_INFO se3_security_info;

/** session buffer */
extern uint8_t se3_sessions_buf[SE3_SESSIONS_BUF];

/** session index */
extern uint8_t* se3_sessions_index[SE3_SESSIONS_MAX];

/** algorithm description table */
extern se3_algo_descriptor algo_table[SE3_ALGO_MAX];
