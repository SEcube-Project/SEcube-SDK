/**
  ******************************************************************************
  * File Name          : L1_error_manager.h
  * Description        : Exception classes that can occur in the L1 library.
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

/*! \file  L1_error_manager.h
 *  \brief This header file defines the exception classes that can occur in the L1 library.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef _L1_ERROR_MANAGER_H
#define _L1_ERROR_MANAGER_H

#include <exception>

class L1Exception : public std::exception {
public:
	virtual const char* what() const throw() override {
		return "General exception in the L1 API";
	}
};

class L1AlreadyOpenException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "SECube already opened";
	}
};

class L1OutOfBoundsException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Pointing outside the vector!";
	}
};

class L1TXRXException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while transmitting data!";
	}
};

class L1PayloadDecryptionException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error In the Payload Decryption!";
	}
};

class L1CommunicationError : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error In the communication!";
	}
};

class L1LoginException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error In the Login!";
	}
};

class L1CryptoSetTimeException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error setting the crypto time!";
	}
};

class L1CryptoInitException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error initializing the crypto session!";
	}
};

class L1CryptoUpdateException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error updating the crypto session!";
	}
};

class L1LogoutException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error logging out!";
	}
};

class L1EncryptException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while encrypting!";
	}
};

class L1DecryptException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while decrypting!";
	}
};

class L1DataIntegrityException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "The integrity and the authenticity of the data cannot be guaranteed.";
	}
};

class L1DigestException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while digesting!";
	}
};

class L1GetAlgorithmsException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while getting algorithms!";
	}
};

class L1ConfigException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while configuring L1!";
	}
};

class L1KeyEditException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while editing the key!";
	}
};

class L1KeyListException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while listing the keys!";
	}
};

class L1FindKeyException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while finding the key!";
	}
};

class L1SelectDeviceException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while changing the device!";
	}
};

class L1GroupEditException : public L1Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while editing the group!";
	}
};

#endif
