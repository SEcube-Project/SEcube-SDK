/**
  ******************************************************************************
  * File Name          : se3c1def.h
  * Description        : L1 constants
  *                      This file contains defs to be used in all the modules
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

#include "se3c0def.h"

enum {
    SE3_ERR_ACCESS = 100,  ///< insufficient privileges
    SE3_ERR_PIN = 101,  ///< pin rejected
    SE3_ERR_RESOURCE = 200,  ///< resource not found
    SE3_ERR_EXPIRED = 201,  ///< resource expired
    SE3_ERR_MEMORY = 400,  ///< no more space to allocate resource
	SE3_ERR_AUTH =	401,	   ///< SHA256HMAC Authentication failed
	SE3_ERR_OPENED = 300   /// < There is a session already opened and host is trying to open a new one
};

/**
 *   @defgroup AccessLogin
 *   @{
 *  	@brief Use this values as access parameter
 *  		   when using login
 */
enum {
    SE3_ACCESS_USER = 100,	
    SE3_ACCESS_ADMIN = 1000,
    SE3_ACCESS_MAX = 0xFFFF
};
/**
 *  @}
 */

/** Configuration records definitions */
enum {
	SE3_RECORD_SIZE = 32,
	SE3_RECORD_MAX = 2
};

/** Default configuration record types */
enum {
    SE3_RECORD_TYPE_ADMINPIN = 0,
    SE3_RECORD_TYPE_USERPIN = 1
};

/** field size definitions */
enum {
    SE3_PIN_SIZE = 32,
    SE3_KEY_SIZE = 32,
    SE3_AUTH_SIZE = 16,
    SE3_CRYPTOBLOCK_SIZE = 16,
    SE3_CHALLENGE_SIZE = 32,
	SE3_CHALLENGE_ITERATIONS = 32,
    SE3_IV_SIZE = 16,
    SE3_TOKEN_SIZE = 16
};

/** request fields definitions */
enum {
    SE3_REQ1_OFFSET_AUTH = 0,
    SE3_REQ1_OFFSET_IV = 16,
    SE3_REQ1_OFFSET_TOKEN = 32,
    SE3_REQ1_OFFSET_LEN = 48,
    SE3_REQ1_OFFSET_CMD = 50,
    SE3_REQ1_OFFSET_DATA = 64,
    SE3_REQ1_MAX_DATA = (SE3_REQ_MAX_DATA - SE3_REQ1_OFFSET_DATA)
};

/** response fields definitions */
enum {
    SE3_RESP1_OFFSET_AUTH = 0,
    SE3_RESP1_OFFSET_IV = 16,
    SE3_RESP1_OFFSET_TOKEN = 32,
    SE3_RESP1_OFFSET_LEN = 48,
    SE3_RESP1_OFFSET_STATUS = 50,
    SE3_RESP1_OFFSET_DATA = 64,
    SE3_RESP1_MAX_DATA = (SE3_RESP_MAX_DATA - SE3_RESP1_OFFSET_DATA)
};

/** command codes */
enum {
    SE3_CMD1_CHALLENGE = 1,
    SE3_CMD1_LOGIN = 2,
    SE3_CMD1_LOGOUT = 3,
    SE3_CMD1_CONFIG = 4,
	SE3_CMD1_KEY_EDIT = 5,
	SE3_CMD1_KEY_FIND = 6,
	SE3_CMD1_KEY_LIST = 7,
	SE3_CMD1_CRYPTO_INIT = 8,
	SE3_CMD1_CRYPTO_UPDATE = 9,
    SE3_CMD1_CRYPTO_LIST = 10,
	SE3_CMD1_LOGOUT_FORCED = 11,
	SE3_CMD1_SEKEY = 12 // added for SEKey
};

/** config operations */
enum {
    SE3_CONFIG_OP_GET = 1,
    SE3_CONFIG_OP_SET = 2
};

/** config fields */
enum {
    SE3_CMD1_CONFIG_REQ_OFF_ID = 0,
    SE3_CMD1_CONFIG_REQ_OFF_OP = 2,
    SE3_CMD1_CONFIG_REQ_OFF_VALUE = 4,
    SE3_CMD1_CONFIG_RESP_OFF_VALUE = 0
};

/** challenge fields */
enum {
    SE3_CMD1_CHALLENGE_REQ_OFF_CC1 = 0,
    SE3_CMD1_CHALLENGE_REQ_OFF_CC2 = 32,
    SE3_CMD1_CHALLENGE_REQ_OFF_ACCESS = 64,
    SE3_CMD1_CHALLENGE_REQ_SIZE = 66,
    SE3_CMD1_CHALLENGE_RESP_OFF_SC = 0,
    SE3_CMD1_CHALLENGE_RESP_OFF_SRESP = 32,
    SE3_CMD1_CHALLENGE_RESP_SIZE = 64
};

/** login fields */
enum {
    SE3_CMD1_LOGIN_REQ_OFF_CRESP = 0,
    SE3_CMD1_LOGIN_REQ_SIZE = 32,
    SE3_CMD1_LOGIN_RESP_OFF_TOKEN = 0,
    SE3_CMD1_LOGIN_RESP_SIZE = 16
};

/** Keys: maximum sizes for variable fields */
enum {
	SE3_KEY_DATA_MAX = 2048,
	SE3_KEY_NAME_MAX = 32,
	SE3_KEY_FINGERPRINT_SIZE = 32,
	SE3_KEY_SALT_SIZE = 32
};

/**
 *  @defgroup KeyOpEdit
 *  @{
 *  @brief Use these values when using \ref key_edit
 */
enum {
    SE3_KEY_OP_ADD = 1,			/**< Use this value to insert a new key passing manually the key value. */
    SE3_KEY_OP_DELETE = 2,		/**< Use this value to delete a key. */
    SE3_KEY_OP_ADD_TRNG = 3		/**< Use this value to insert a key generated with the TRNG of the SEcube. */
};
/**
 *  @}
 */

/** key_edit fields */
enum {
    SE3_CMD1_KEY_EDIT_REQ_OFF_OP = 0,
    SE3_CMD1_KEY_EDIT_REQ_OFF_ID = 2,
    SE3_CMD1_KEY_EDIT_REQ_OFF_DATA_LEN = 6,
    SE3_CMD1_KEY_EDIT_REQ_OFF_DATA = 8
};

/** L1_key_list fields */
enum {
    SE3_CMD1_KEY_LIST_REQ_SIZE = 36,
    SE3_CMD1_KEY_LIST_REQ_OFF_SKIP = 0,
    SE3_CMD1_KEY_LIST_REQ_OFF_NMAX = 2,
	SE3_CMD1_KEY_LIST_REQ_OFF_SALT = 4,
    SE3_CMD1_KEY_LIST_RESP_OFF_COUNT = 0,
    SE3_CMD1_KEY_LIST_RESP_OFF_KEYINFO = 2,

    SE3_CMD1_KEY_LIST_KEYINFO_OFF_ID = 0,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_DATA_LEN = 4,
	SE3_CMD1_KEY_LIST_KEYINFO_OFF_FINGERPRINT = 6,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_NAME = 38
};

/** Invalid handle values */
enum {
	SE3_ALGO_INVALID = 0xFFFF,
	SE3_SESSION_INVALID = 0xFFFFFFFF,
	SE3_KEY_INVALID = 0xFFFFFFFF // SE3_KEY_INVALID (value 0xFFFFFFFF) must be passed to crypto_init whenever a key is NOT needed (i.e. SHA-256)
};

/**
 *  \defgroup AlgorithmAvail
 *  @{
 */
enum {
    SE3_ALGO_AES = 0,  ///< AES
	SE3_ALGO_SHA256 = 1,  ///< SHA256
	SE3_ALGO_HMACSHA256 = 2,  ///< HMAC-SHA256
	SE3_ALGO_AES_HMACSHA256 = 3,  ///< AES + HMAC-SHA256
    SE3_ALGO_MAX = 4
};
/**
 *  @}
 */

/** crypto_init fields */
enum {
    SE3_CMD1_CRYPTO_INIT_REQ_SIZE = 8,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_ALGO = 0,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_MODE = 2,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_KEY_ID = 4,
    SE3_CMD1_CRYPTO_INIT_RESP_SIZE = 4,
    SE3_CMD1_CRYPTO_INIT_RESP_OFF_SID = 0
};

/** crypto_update fields */
enum {
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID = 0,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS = 4,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN1_LEN = 6,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN2_LEN = 8,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA = 16,
    SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATAOUT_LEN = 0,
    SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA = 16
};

/** crypto_update default flags */
enum {
	SE3_CRYPTO_FLAG_FINIT = (1 << 15),
	SE3_CRYPTO_FLAG_RESET = (1 << 14),
	SE3_CRYPTO_FLAG_SETIV = SE3_CRYPTO_FLAG_RESET,
	SE3_CRYPTO_FLAG_SETNONCE = (1 << 13),
	SE3_CRYPTO_FLAG_AUTH = (1 << 12)
};

/** crypto_update maximum buffer sizes */
enum {
    SE3_CRYPTO_MAX_DATAIN = (SE3_REQ1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA),
    SE3_CRYPTO_MAX_DATAOUT = (SE3_RESP1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA)
};

/** crypto_set_time fields */
enum {
    SE3_CMD1_CRYPTO_SET_TIME_REQ_SIZE = 4,
    SE3_CMD1_CRYPTO_SET_TIME_REQ_OFF_DEVTIME = 0
};

/** crypto_list fields */
enum {
    SE3_CMD1_CRYPTO_LIST_REQ_SIZE = 0,
    SE3_CMD1_CRYPTO_LIST_RESP_OFF_COUNT = 0,
    SE3_CMD1_CRYPTO_LIST_RESP_OFF_ALGOINFO = 2,
    //SE3_CMD1_CRYPTO_ALGOINFO_SIZE = 22,
	SE3_CMD1_CRYPTO_ALGOINFO_SIZE = 40, // SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE + 2 (display_type) + 2 (display_block_size) + SE3_CMD1_CRYPTO_ALGOINFO_KEY_SIZE
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_NAME = 0,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_TYPE = 16,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_BLOCK_SIZE = 18,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_KEY_SIZE = 20,
    SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE = 16,
	SE3_CMD1_CRYPTO_ALGOINFO_KEY_NUM = 10, // how many key sizes for each algo struct
	SE3_CMD1_CRYPTO_ALGOINFO_KEY_SIZE = 20 // 10 * uint16_t = 20 byte
};

/** crypto_list default cipher types */
enum {
	SE3_CRYPTO_TYPE_BLOCKCIPHER = 0,
	SE3_CRYPTO_TYPE_STREAMCIPHER = 1,
	SE3_CRYPTO_TYPE_DIGEST = 2,
	SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH = 3,
	SE3_CRYPTO_TYPE_OTHER = 0xFFFF
};

#define SE3_DIR_SHIFT (8)
/**  \brief crypto_init default modes
  *  
  *  One FEEDBACK and one DIR may be combined to specify the desired mode
  *  Example:
  *     Encrypt in CBC mode
  *     (SE3_FEEDBACK_CBC | SE3_DIR_ENCRYPT) 
  */
enum {
	SE3_FEEDBACK_ECB = 1,
	SE3_FEEDBACK_CBC = 2,
	SE3_FEEDBACK_OFB = 3,
	SE3_FEEDBACK_CTR = 4,
	SE3_FEEDBACK_CFB = 5,
	SE3_DIR_ENCRYPT = (1 << SE3_DIR_SHIFT),
	SE3_DIR_DECRYPT = (2 << SE3_DIR_SHIFT)
};

/* KEY IDs:
 * 0 -> not allowed
 * [1 - 999] manual key management (no SEkey)
 * [1000 - 1999] reserved for SEcube internal purposes
 * [2000 - 4.294.867.293] automatic key management (SEkey)
 * [4.294.867.294 - 4.294.967.294] reserved for SEkey internal purposes
 * UINT32_MAX (4.294.967.295) -> not allowed
 * Warning: using define instead of enum because enum type (signed or unsigned) is implementation dependent. */
#define	ZERO_ID 0U /**< Key ID 0 is not allowed. */
#define	MANUAL_ID_BEGIN 1U /**< Keys that can be managed manually, without KMS (SEkey), start from ID = 1 up to ID = 999 */
#define	MANUAL_ID_END 999U /**< Keys that can be managed manually, without KMS (SEkey), start from ID = 1 up to ID = 999 */
#define	RESERVED_ID_SECUBE_BEGIN 1000U /**< Keys created for internal purposes of the SEcube or of any SEcube-related library (i.e. SEkey). Range [1000-1999]. */
#define	RESERVED_ID_SECUBE_END 1999U /**< Keys created for internal purposes of the SEcube or of any SEcube-related library (i.e. SEkey). Range [1000-1999]. */
// these 3 are pre-allocated IDs used for internal purposes (in fact they are inside the reserved secube range)
#define	RESERVED_ID_SEKEY_SECUREDB 1001U /**< Key ID 1001 is reserved to encrypt the database internally used by SEkey. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */
#define	RESERVED_ID_SEKEY_WILDCARD 1002U /**< Key ID 1002 is reserved for SEkey internal purposes. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */
#define	RESERVED_ID_SETELEGRAM 1003U /**< Key ID 1003 is reserved to encrypt the SEtelegram database. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */
#define	RESERVED_ID_SEKEY_BEGIN UINT32_MAX - 1U - 100000U /**< Keys in range [4.294.867.294 - 4.294.967.294] are reserved for special internal purposes of SEkey. */
#define	RESERVED_ID_SEKEY_END UINT32_MAX - 1U /**< Keys in range [4.294.867.294 - 4.294.967.294] are reserved for special internal purposes of SEkey. */
#define	SEKEY_ID_BEGIN 2000U /**< IDs managed by the SEkey KMS are in range [2000 -  4.294.867.293]. */
#define	SEKEY_ID_END RESERVED_ID_SEKEY_BEGIN - 1U /**< IDs managed by the SEkey KMS are in range [2000 -  4.294.867.293]. */
#define	NULL_ID UINT32_MAX /**< This value is provided to the functions that required a key id as parameter but don't actual need a key to perform the underlying algorithm; for instance L1CryptoInit() for SHA-256. */


// required for in-place encryption with AES
#if (SE3_CRYPTO_MAX_DATAIN % 16 != 0)
#error "SE3_CRYPTO_MAX_DATAIN is not a multiple of 16"
#endif
#if (SE3_CRYPTO_MAX_DATAOUT % 16 != 0)
#error "SE3_CRYPTO_MAX_DATAIN is not a multiple of 16"
#endif
#if (SE3_REQ1_MAX_DATA % 16 != 0)
#error "SE3_REQ1_MAX_DATA is not a multiple of 16"
#endif
#if (SE3_RESP1_MAX_DATA % 16 != 0)
#error "SE3_RESP1_MAX_DATA is not a multiple of 16"
#endif
#if (SE3_REQ1_OFFSET_DATA % 16 != 0)
#error "SE3_REQ1_OFFSET_DATA is not a multiple of 16"
#endif
#if (SE3_RESP1_OFFSET_DATA % 16 != 0)
#error "SE3_RESP1_OFFSET_DATA is not a multiple of 16"
#endif
