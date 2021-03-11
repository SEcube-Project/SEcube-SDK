/**
  ******************************************************************************
  * File Name          : L0_provision.cpp
  * Description        : Implementation of the PROVISION API.
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

/**
 * @file	L0_provision.cpp
 * @date	July, 2017
 * @brief	Implementation of the PROVISION API
 *
 * The file contains the implementation of the PROVISION API
 */

#include "L0.h"
#include "L0_error_manager.h"

uint16_t L0::L0FactoryInit(const std::array<uint8_t, L0Communication::Size::SERIAL>& serialno) {
	uint16_t respStatus = 0;
	uint16_t respLen = 0;
	L0FactoryInitException factInitExc;
	try {
		L0TXRX(L0Commands::Command::FACTORY_INIT, 0, L0Communication::Size::SERIAL, serialno.data(), &respStatus, &respLen, NULL);
	}
	catch (L0Exception& e) {
		throw factInitExc;
	}
	return respStatus;
}
