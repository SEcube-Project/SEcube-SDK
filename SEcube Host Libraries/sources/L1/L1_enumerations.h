/**
  ******************************************************************************
  * File Name          : L1_enumerations.h
  * Description        : Enumerations related to the L1 library.
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

/*! \file  L1_enumerations.h
 *  \brief This header file defines the enumerations related to the L1 library.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef L1_ENUMERATIONS_H
#define L1_ENUMERATIONS_H

#include "../L0/L0_enumerations.h"

namespace L1Error {
	struct Error {
		enum {
		    OK = 0, 					/**< No Error. */
		    SE3_ERR_ACCESS = 100,  		/**< Insufficient privilege. */
		    SE3_ERR_PIN = 101,  		/**< Wrong PIN. */
		    SE3_ERR_RESOURCE = 200, 	/**< Resource not found. */
		    SE3_ERR_EXPIRED = 201,  	/**< Resource expired. */
		    SE3_ERR_MEMORY = 400,  		/**< No more space to allocate resource on the device. */
			SE3_ERR_AUTH =	401,	  	/**< HMAC-SHA-256 authentication failed. */
		    SE3_ERR_OPENED = 300   		/**< There is already an open session on the SEcube (L1Login() already done, without performing L1Logout() afterwards) and the host is trying to open a new one. */
		};
	};
}

namespace L1Request {
	/** Offset values of different fields into the request payload to be sent to the SEcube. */
	struct Offset {
		enum {
			AUTH = 0,
			IV = 16,
			TOKEN = 32,
			LEN = 48,
			CMD = 50,
			DATA = 64,
		};
	};

	struct Size {
		enum {
			MAX_DATA = L0Request::Size::MAX_DATA - L1Request::Offset::DATA /**< Maximum quantity of data that can be sent in a request to the SEcube. */
		};
	};

struct KeyOffset {
	enum {
		OP = 0,
		ID = 2,
		DATA_LEN = 6,
		DATA = 8
	};
};
}

namespace L1Response {
	/** Offset values of different fields into the reply payload received from the SEcube. */
	struct Offset {
		enum {
			AUTH = 0,
			IV = 16,
			TOKEN = 32,
			LEN = 48,
			STATUS = 50,
			DATA = 64
		};
	};

	struct Size {
		enum {
			MAX_DATA = L0Response::Size::MAX_DATA - L1Response::Offset::DATA /**< Maximum quantity of data that can be sent in a reply from the SEcube. */
		};
	};
}

namespace L1ChallengeRequest {
	struct Offset {
		enum {
			//SE3_CMD1_CHALLENGE_REQ_OFF_CC1 = 0,
			CC1 = 0,
			//SE3_CMD1_CHALLENGE_REQ_OFF_CC2 = 32,
			CC2 = 32,
			//SE3_CMD1_CHALLENGE_REQ_OFF_ACCESS = 64
			ACCESS = 64
		};
	};

	struct Size {
		enum {
			//SE3_CMD1_CHALLENGE_REQ_SIZE = 66
			SIZE = 66
		};
	};
}

namespace L1ChallengeResponse {
	struct Offset {
		enum {
			//SE3_CMD1_CHALLENGE_RESP_OFF_SC = 0,
			SC = 0,
			//SE3_CMD1_CHALLENGE_RESP_OFF_SRESP = 32
			SRESP = 32
		};
	};

	struct Size {
		enum {
			//SE3_CMD1_CHALLENGE_RESP_SIZE = 64
			SIZE = 64
		};
	};
}

namespace L1Parameters {
	struct Size {
		enum {
			//SE3_L1_PIN_SIZE = 32,
			PIN = 32,
			//SE3_L1_KEY_SIZE = 32,
			KEY = 32,
			//SE3_L1_AUTH_SIZE = 16,
			AUTH = 16,
			//SE3_L1_CRYPTOBLOCK_SIZE = 16,
			CRYPTO_BLOCK = 16,
			//SE3_L1_CHALLENGE_SIZE = 32,
			CHALLENGE = 32,
			//SE3_L1_IV_SIZE = 16,
			IV = 16,
			//SE3_L1_TOKEN_SIZE = 16
			TOKEN = 16
		};
	};

	struct Parameter {
		enum {
			//SE3_L1_CHALLENGE_ITERATIONS = 32
			ITERATIONS = 32
		};
	};
}

namespace L1Crypto {
	/** Types of crypto algorithms. */
	struct CryptoTypes {
		enum {
			SE3_CRYPTO_TYPE_BLOCKCIPHER = 0, /**< Block cipher. */
			SE3_CRYPTO_TYPE_STREAMCIPHER = 1, /**< Stream cipher. */
			SE3_CRYPTO_TYPE_DIGEST = 2, /**< Digest algorithm (i.e. SHA-256, HMAC-SHA-256). */
			SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH = 3, /**< Block cipher algorithm, granting also integrity and authentication (i.e. AES-256-HMAC-SHA-256). */
			SE3_CRYPTO_TYPE_OTHER = 0xFFFF /**< Anything else not listed above. */
		};
	};

	struct ListResponseOffset {
		enum {
			//SE3_CMD1_CRYPTO_LIST_RESP_OFF_COUNT = 0,
			COUNT = 0,
			//SE3_CMD1_CRYPTO_LIST_RESP_OFF_ALGOINFO = 2
			ALGORITHM_INFO = 2
		};
	};

	struct ListRequestSize {
		enum {
			//SE3_CMD1_CRYPTO_LIST_REQ_SIZE = 0,
			REQ_SIZE = 0,
		};
	};

	struct AlgorithmInfoOffset {
		enum {
			//SE3_CMD1_CRYPTO_ALGOINFO_OFF_NAME = 0,
			NAME = 0,
			//SE3_CMD1_CRYPTO_ALGOINFO_OFF_TYPE = 16,
			TYPE = 16,
			//SE3_CMD1_CRYPTO_ALGOINFO_OFF_BLOCK_SIZE = 18,
			BLOCK_SIZE = 18,
			//SE3_CMD1_CRYPTO_ALGOINFO_OFF_KEY_SIZE = 20
			KEY_SIZE = 20
		};
	};

	struct AlgorithmInfoSize {
		enum {
			KEY_NUM = 10,
			SIZE = 40,
			NAME_SIZE = 16,
			KEY_SIZE = 20
		};
	};

	struct SetTimeRequestOffset {
		enum {
			//SE3_CMD1_CRYPTO_SET_TIME_REQ_OFF_DEVTIME = 0
			DEV_TIME = 0
		};
	};

	struct SetTimeRequestSize {
		enum {
			//SE3_CMD1_CRYPTO_SET_TIME_REQ_SIZE = 4
			SIZE = 4
		};
	};

	struct InitRequestOffset {
		enum {
			//SE3_CMD1_CRYPTO_INIT_REQ_OFF_ALGO = 0,
			ALGO = 0,
			//SE3_CMD1_CRYPTO_INIT_REQ_OFF_MODE = 2,
			MODE = 2,
			//SE3_CMD1_CRYPTO_INIT_REQ_OFF_KEY_ID = 4
			KEY_ID = 4
		};
	};

	struct InitRequestSize {
		enum {
			//SE3_CMD1_CRYPTO_INIT_REQ_SIZE = 8
			SIZE = 8
		};
	};

	struct InitResponseOffset {
		enum {
			//SE3_CMD1_CRYPTO_INIT_RESP_OFF_SID = 0
			SID = 0
		};
	};

	struct InitResponseSize {
		enum {
			//SE3_CMD1_CRYPTO_INIT_RESP_SIZE = 4
			SIZE = 4
		};
	};

	struct UpdateRequestOffset {
		enum {
			//SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID = 0,
			SID = 0,
			//SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS = 4,
			FLAGS = 4,
			//SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN1_LEN = 6,
			DATAIN1_LEN = 6,
			//SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN2_LEN = 8,
			DATAIN2_LEN = 8,
			//SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA = 16
			DATA = 16
		};
	};

	struct UpdateResponseOffset {
		enum {
			//SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATAOUT_LEN = 0,
			DATAOUT_LEN = 0,
			//SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA = 16
			DATA = 16
		};
	};

	struct UpdateSize {
		enum {
			//SE3_CRYPTO_MAX_DATAIN = (SE3_REQ1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA),
			DATAIN = L1Request::Size::MAX_DATA - L1Crypto::UpdateRequestOffset::DATA,
			//SE3_CRYPTO_MAX_DATAOUT = (SE3_RESP1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA)
			DATAOUT = L1Response::Size::MAX_DATA - L1Crypto::UpdateResponseOffset::DATA
		};
	};

	struct UpdateFlags {
		enum {
			FINIT = 1 << 15, 	/**< This flag is used to finalize the crypto operation. It must be used, for example, when working on the last block of data. */
			RESET = 1 << 14, 	/**< Same as SET_IV. */
			SET_IV = RESET, 	/**< This flag is used to set the initialization vector for the given algorithm and mode. */
			SETNONCE = 1 << 13, /**< This flag is used to setup the nonce that must be used for a given crypto operation. */
			AUTH = 1 << 12 		/**< This flag enables the digest computation for AES-HMAC-SHA256. */
		};
	};
}

namespace L1Key {
	struct IdClass {
		enum {
			INVALID = 0,
			MANUAL = 1,
			RESERVED_SECUBE = 2,
			RESERVED_SEKEY = 3,
			KMS = 4
		};
	};

	struct Id {
		/* KEY IDs:
		 * 0 -> not allowed
		 * [1 - 999] manual key management (no SEkey)
		 * [1000 - 1999] reserved for SEcube internal purposes
		 * [2000 - 4.294.867.293] automatic key management (SEkey)
		 * [4.294.867.294 - 4.294.967.294] reserved for SEkey internal purposes
		 * UINT32_MAX (4.294.967.295) -> not allowed */
		enum : uint32_t {
			ZERO_ID = 0, /**< Key ID 0 is not allowed. */
			MANUAL_ID_BEGIN = 1, /**< Keys that can be managed manually, without KMS (SEkey), start from ID = 1 up to ID = 999 */
			MANUAL_ID_END = 999, /**< Keys that can be managed manually, without KMS (SEkey), start from ID = 1 up to ID = 999 */
			RESERVED_ID_SECUBE_BEGIN = 1000, /**< Keys created for internal purposes of the SEcube or of any SEcube-related library (i.e. SEkey). Range [1000-1999]. */
			RESERVED_ID_SECUBE_END = 1999, /**< Keys created for internal purposes of the SEcube or of any SEcube-related library (i.e. SEkey). Range [1000-1999]. */

			// these 3 are pre-allocated IDs used for internal purposes (in fact they are inside the reserved secube range)
			RESERVED_ID_SEKEY_SECUREDB = 1001, /**< Key ID 1001 is reserved to encrypt the database internally used by SEkey. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */
			RESERVED_ID_SEKEY_WILDCARD = 1002, /**< Key ID 1002 is reserved for SEkey internal purposes. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */
			RESERVED_ID_SETELEGRAM = 1003, /**< Key ID 1003 is reserved to encrypt the SEtelegram database. Notice that this is in the range [1000-1999] that is reserved for SEcube's internal purposes. */

			RESERVED_ID_SEKEY_BEGIN = UINT32_MAX - 1 - 100000, /**< Keys in range [4.294.867.294 - 4.294.967.294] are reserved for special internal purposes of SEkey. */
			RESERVED_ID_SEKEY_END = UINT32_MAX - 1, /**< Keys in range [4.294.867.294 - 4.294.967.294] are reserved for special internal purposes of SEkey. */
			SEKEY_ID_BEGIN = 2000, /**< IDs managed by the SEkey KMS are in range [2000 -  4.294.867.293]. */
			SEKEY_ID_END = RESERVED_ID_SEKEY_BEGIN - 1, /**< IDs managed by the SEkey KMS are in range [2000 -  4.294.867.293]. */
			NULL_ID = UINT32_MAX /**< This value is provided to the functions that required a key id as parameter but don't actual need a key to perform the underlying algorithm; for instance L1CryptoInit() for SHA-256. */
		};
	};

	struct Size {
		enum {
			//SE3_KEY_DATA_MAX = 2048,
			MAX_DATA = 2048,
			//SE3_KEY_NAME_MAX = 32
			MAX_NAME = 32
		};
	};

	struct RequestListSize {
		enum {
			//SE3_CMD1_KEY_LIST_REQ_SIZE = 36
			SIZE = 36
		};
	};

	struct RequestListOffset {
		enum {
			//SE3_CMD1_KEY_LIST_REQ_OFF_SKIP = 0,
			SKIP = 0,
			//SE3_CMD1_KEY_LIST_REQ_OFF_NMAX = 2,
			NMAX = 2,
		};
	};

	struct ResponeListOffset {
		enum {
			//SE3_CMD1_KEY_LIST_RESP_OFF_COUNT = 0,
			COUNT = 0,
			//SE3_CMD1_KEY_LIST_RESP_OFF_KEYINFO = 2
			KEYINFO = 2
		};
	};

	struct InfoOffset {
		enum {
			//SE3_CMD1_KEY_LIST_KEYINFO_OFF_ID = 0,
			ID = 0,
			//SE3_CMD1_KEY_LIST_KEYINFO_OFF_VALIDITY = 4,
			VALIDITY = 4,
			//SE3_CMD1_KEY_LIST_KEYINFO_OFF_DATA_LEN = 8,
			DATA_LEN = 8,
			//SE3_CMD1_KEY_LIST_KEYINFO_OFF_NAME_LEN = 10,
			NAME_LEN = 10,
			//SE3_CMD1_KEY_LIST_KEYINFO_OFF_FINGERPRINT = 12
			FINGERPRINT = 12,
			// SE3_CMD1_KEY_LIST_KEYINFO_OFF_NAME = 44
			NAME = 44

		};
	};
}

namespace L1Commands {
	/** These flags are used in L1 APIs to encrypt and sign the requests that are sent to the SEcube.
	 *  The encryption and signature process triggered by these flags seems to be the cause of random
	 *  errors when the host-side code runs on Unix operating systems (i.e. Ubuntu). The same behavior
	 *  has not been noticed on Windows. For the moment, it is suggested to avoid using them. */
	struct Flags {
		enum {
			ENCRYPT = 1 << 15, /**< encrypt L1 packet sent to SEcube */
			SIGN = 1 << 14	   /**< sign L1 packet sent to SEcube */
		};
	};

	/** These are the codes used to identify the commands sent to the SEcube. You can add new codes to define new commands supported by the SEcube firmware. */
	struct Codes {
		enum {
			CHALLENGE = 1,
			LOGIN = 2,
			LOGOUT = 3,
			CONFIG = 4,
			KEY_EDIT = 5,
			KEY_FIND = 6,
			KEY_LIST = 7,
			CRYPTO_INIT = 8,
			CRYPTO_UPDATE = 9,
			CRYPTO_LIST = 10,
			FORCED_LOGOUT=11,
			SEKEY = 12
		};
	};

	/** These codes are used to select a specific sub-operation identified by L1Commands::Codes::SEKEY. They are used only to implement basic
	 * firmware features required by SEkey. */
	struct Options {
		enum{
			SE3_SEKEY_OP_SETINFO = 1,
			SE3_SEKEY_OP_GETINFO = 2,
			SE3_SEKEY_OP_GETKEY = 3,
			SE3_SEKEY_OP_GET_KEY_IDS = 4,
			SE3_SEKEY_DELETEALL = 5,
			SE3_SEKEY_DELETEKEY = 6,
			SE3_SEKEY_OP_GETKEYENC = 7,
			SE3_SEKEY_INSERTKEY = 8,
			SE3_SEKEY_ISREADY = 9
		};
	};

	struct KeyOpEdit {
		enum {
		    SE3_KEY_OP_ADD = 1,			/**< Use this value to insert a new key passing manually the key value. */
		    SE3_KEY_OP_DELETE = 2,		/**< Use this value to delete a key. */
		    SE3_KEY_OP_ADD_TRNG = 3		/**< Use this value to insert a key generated with the TRNG of the SEcube. */
		};
	};
}

namespace L1Login {
	struct RequestOffset {
		enum {
			//SE3_CMD1_LOGIN_REQ_OFF_CRESP = 0
			CRESP = 0
		};
	};

	struct RequestSize {
		enum {
			//SE3_CMD1_LOGIN_REQ_SIZE = 32
			SIZE = 32
		};
	};

	struct ResponseOffset {
		enum {
			//SE3_CMD1_LOGIN_RESP_OFF_TOKEN = 0
			TOKEN = 0
		};
	};

	struct ResponseSize {
		enum {
			//SE3_CMD1_LOGIN_RESP_SIZE = 16
			SIZE = 16
		};
	};
}

/* WARNING: SEkey APIs are based on these values. If you change them, make sure to change also the code in SEkey accordingly. */
namespace L1Algorithms {
	/** Algorithms supported by the SEcube device. */
	struct Algorithms {
		enum {
		    AES = 0,  			/**< AES cipher */
			SHA256 = 1,  		/**< SHA-256 digest  */
			HMACSHA256 = 2,  	/**< HMAC-SHA-256 digest */
			AES_HMACSHA256 = 3, /**< AES + HMAC + SHA-256 */
			ALGORITHM_MAX = 4	/**< Value required by SEfile */
		};
	};
}

namespace L1Configuration {
	struct RequestOffset {
		enum {
			//SE3_CMD1_CONFIG_REQ_OFF_ID = 0,
			CONFIG_ID = 0,
			//SE3_CMD1_CONFIG_REQ_OFF_OP = 2,
			CONFIG_OP = 2,
			//SE3_CMD1_CONFIG_REQ_OFF_VALUE = 4,
			CONFIG_VALUE = 4,
		};
	};

	struct ResponseOffset {
		enum {
			//SE3_CMD1_CONFIG_RESP_OFF_VALUE = 0
			CONFIG_VALUE = 0
		};
	};

	struct Operation {
		enum {
			//SE3_CONFIG_OP_GET = 1,
			GET = 1,
			//SE3_CONFIG_OP_SET = 2
			SET = 2
		};
	};

	struct RecordSize {
		enum {
			//SE3_RECORD_SIZE = 32,
			RECORD_SIZE = 32,
			//SE3_RECORD_MAX = 2
			RECORD_MAX = 2
		};
	};

	struct RecordType {
		enum {
			ADMINPIN = 0, // SE3_RECORD_TYPE_ADMINPIN = 0
			USERPIN = 1 // SE3_RECORD_TYPE_USERPIN = 1
		};
	};
}

namespace CryptoInitialisation {
	struct Parameters {
		enum {
			SHIFT = 8 // SE3_DIR_SHIFT = 8
		};
	};

	/** Direction of cryptographic algorithm. */
	struct Direction {
		enum {
			ENCRYPT = 1 << CryptoInitialisation::Parameters::SHIFT, /**< Setup crypto for encryption of data. */
			DECRYPT = 2 << CryptoInitialisation::Parameters::SHIFT  /**< Setup crypto for decryption of data. */
		};
	};

	/** Mode of usage of AES. */
	struct Modes {
		enum {
			ECB = 1, /**< Not very strong. Don't use it, if possible. */
			CBC = 2, /**< If you were thinking about using ECB, CBC is a far better choice. */
			OFB = 3, /**< Turns AES into a stream cipher. Similar to CFB. */
			CTR = 4, /**< Turns AES into a stream cipher. By far the most used. */
			CFB = 5,	 /**< Turns AES into a stream cipher. Similar to OFB. */
			INVALID_AES_MODE = 6 /**< Just a value to identify an invalid mode. */
		};
	};
}

/** @brief Here are the constants that are used by the SEkey API. */
namespace L1SEkey {
	struct Direction { /**< Operations to read or write SEkey user info (user name and user ID) from the SEcube to the host and vice-versa */
		enum {
			LOAD, /**< Read from the SEcube */
			STORE /**< Write to the SEcube */
		};
	};
}

#endif
