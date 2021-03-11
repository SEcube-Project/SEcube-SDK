/**
  ******************************************************************************
  * File Name          : L0_enumerations.h
  * Description        : Enumerations related to the L0 library.
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

/*! \file  L0_enumerations.h
 *  \brief This header file defines the enumerations related to the L0 library.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef _L0_ENUMERATIONS_H
#define _L0_ENUMERATIONS_H

#ifdef _WIN32
	#include "Windows.h"
#endif

namespace L0Communication {
	struct Size {
		enum {
			MAGIC = 32, /**< Size of magic block for SEcube low level communications (bytes) */
			HELLO = 32, /**< Size of SEcube hello message (bytes) */
			SERIAL = 32 /**< Size of SEcube serial number (bytes) */
		};
	};

	struct Parameter {
		enum {
			COMM_BLOCK = 512,
			COMM_N = 16,
			SE3_MAX_PATH = 256 // MAX_PATH is already defined in Windows.h
		};
	};

	struct Error {
		enum {
			OK = 0,
			ERR_NOT_FOUND = 1,
			ERR_TIMEOUT = 2,
			ERR_NO_DEVICE = 3
		};
	};
}

namespace L0DiscoverParameters {
	struct Offset {
		enum {
			SE3_DISCO_OFFSET_MAGIC = 0,
			SERIAL = 32,
			HELLO = 2 * 32,
			STATUS = 3 * 32
		};
	};
}

namespace L0Request {
	struct Size {
		enum {
			HEADER = 16,
			DATA_HEADER = 4,
			SE3_REQ_SIZE_DATA = L0Communication::Parameter::COMM_BLOCK - HEADER,
			SE3_REQDATA_SIZE_DATA = L0Communication::Parameter::COMM_BLOCK - DATA_HEADER,
			MAX_DATA = L0Communication::Parameter::COMM_BLOCK - HEADER + (L0Communication::Parameter::COMM_N - 2) * (L0Communication::Parameter::COMM_BLOCK - DATA_HEADER) - 8

		};
	};

	struct Offset {
		enum {
			CMD = 0,		//offset of the command
			CMD_FLAGS = 2,	//offset of the command flags
			LEN = 4,		//offset to write the length of the data and the header
			CMD_TOKEN = 6,	//offset of the command token
			PADDING = 10,	//offset for the padding
			CRC = 14,		//offset of the CRC
			DATA = 16,
			DATA_CMD_TOKEN = 0,
			SE3_REQDATA_OFFSET_DATA = 4
		};
	};
}

namespace L0Response {
	struct Offset {
		enum {
			SE3_RESP_OFFSET_READY = 0,
			STATUS = 2,
			LEN = 4,
			CMD_TOKEN = 6,
			SE3_RESP_OFFSET_CRC = 14,
			DATA_CMD_TOKEN = 0,
			SE3_RESPDATA_OFFSET_DATA = 4
		};
	};

	struct Size {
		enum {
			HEADER = 16,
			DATA_HEADER = 4,
			SE3_RESP_SIZE_DATA = L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER,
			SE3_RESPDATA_SIZE_DATA = L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER,
			MAX_DATA = L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER + (L0Communication::Parameter::COMM_N - 2) * (L0Communication::Parameter::COMM_BLOCK - L0Request::Size::DATA_HEADER) - 8
		};
	};
}

namespace L0Commands {
	struct Command {
		enum {
			FACTORY_INIT = 1,
			ECHO = 2,
			L1_CMD0 = 3,
			SE3_CMD0_BOOT_MODE_RESET = 4
		};
	};
}

namespace L0ErrorCodes {
	struct Error {
		enum {
			OK = 0,						/**< success (alias SE3_OK in the firmware) */
			SE3_ERR_HW = 0xF001,  		/**< hardware failure */
			COMMUNICATION = 0xF002,		/**< communication errors (alias SE3_ERR_COMM) */
			SE3_ERR_BUSY = 0xF003,  	/**< device locked by another process */
			SE3_ERR_STATE = 0xF004,  	/**< invalid state for this operation */
			SE3_ERR_CMD = 0xF005,  		/**< command does not exist */
			SE3_ERR_PARAMS = 0xF006,  	/**< parameters are not valid */
		};
	};
}

// for generating random values in WINDOWS environment
#ifdef _WIN32
namespace L0Win32ApiCodes {
	struct Codes {
		enum {
			FULL = 1, // ADVAPI32_PROV_RSA_FULL = 1
			NEW_KEYSET = 0x00000008, // ADVAPI32_CRYPT_NEWKEYSET = 0x00000008
			BAD_KEYSET = _HRESULT_TYPEDEF_(0x80090016L) // ADVAPI32_CRYPT_NEWKEYSET = 0x00000008
		};
	};
}
#endif

#endif
