/**
  ******************************************************************************
  * File Name          : communication_api.h
  * Description        : Prototypes of the COMMUNICATION API.
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

/*! \file  communication_api.h
 *  \brief Prototypes of the COMMUNICATION API.
 *  \version SEcube Open Source SDK 1.5.1
 */

#include "../L0 Base/L0_base.h"

class CommunicationApi {
private:
	virtual bool Se3Open(uint64_t deadline, se3File* phFile, se3DiscoverInfo* disco) = 0;
	virtual uint16_t L0TX(uint16_t cmd, uint16_t cmdFlags, uint16_t len, const uint8_t* data) = 0;
	virtual uint16_t L0RX(uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) = 0;
public:
	virtual ~CommunicationApi(){};
	/** @brief Open communication with SEcube device. */
	virtual void L0Open() = 0;
	/** @brief Close communication with SEcube device. */
	virtual void L0Close() = 0;
	/** @brief For internal usage of L0 and L1 levels. */
	virtual void L0Open(uint8_t devPtr) = 0;
	/** @brief For internal usage of L0 and L1 levels. */
	virtual void L0Close(uint8_t devPtr) = 0;
	/** @brief Send command to SEcube, wait for reply, read reply (low level, used also by L1TXRX()). */
	virtual void L0TXRX(uint16_t reqCmd, uint16_t reqCmdFlags, uint16_t reqLen, const uint8_t* reqData, uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) = 0;
	/** @brief The SEcube echoes back any data it receives. */
	virtual uint16_t L0Echo(const uint8_t* dataIn, uint16_t dataInLen, uint8_t* dataOut) = 0;
};
