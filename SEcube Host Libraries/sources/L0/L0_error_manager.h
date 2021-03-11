/**
  ******************************************************************************
  * File Name          : L0_error_manager.h
  * Description        : Exception classes that can occur in the L0 library.
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

/*! \file  L0_error_manager.h
 *  \brief This header file defines the exception classes that can occur in the L0 library.
 *  \version SEcube Open Source SDK 1.5.1
 */

#include <iostream>
#include <exception>

class L0Exception : public std::exception {
public:
	virtual const char* what() const throw() override {
		return "General exception in the L0 API";
	}
};

class L0NoDeviceException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "No device at the specified pointer!";
	}
};

class L0CommunicationErrorException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Communication error!";
	}
};

class L0TXRXException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Error while transmitting data!";
	}
};

class L0NoDeviceOpenedException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "No device opened!";
	}
};

class L0ParametersErrorException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Parameter Error!";
	}
};

class L0TXException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "TX Error!";
	}
};

class L0RXException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "RX Error!";
	}
};

class L0FactoryInitException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Exception while performing the factory initialization!";
	}
};

class DeviceAlreadyInitializedException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Device has already been initialized!";
	}
};

class L0EchoException : public L0Exception {
public:
	virtual const char* what() const throw() override {
		return "Echo exception!";
	}
};
