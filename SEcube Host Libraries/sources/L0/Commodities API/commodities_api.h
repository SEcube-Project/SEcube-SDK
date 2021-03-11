/**
  ******************************************************************************
  * File Name          : commodities_api.h
  * Description        : Prototypes of the COMMODITY API.
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

/*! \file  commodities_api.h
 *  \brief Prototypes of the COMMODITY API
 *  \version SEcube Open Source SDK 1.5.1
 *  \date	July, 2017
 */

#include "../L0 Base/L0_base.h"

/* Pure virtual class. L0.h contains the prototype, while the L0_commodities.cpp contains the implementation */
class CommoditiesApi {
private:
	/* SUPPORT METHODS FOR THE COMMODITIES API */
	virtual bool Se3DriveNext() = 0;
	virtual bool Se3Info(uint64_t deadline, se3DiscoverInfo* info) = 0;
	/** @brief Initialize iterator to discover SEcube devices connected to the PC. */
	virtual void L0DiscoverInit() = 0;
	/** @brief Iterate through the SEcube devices connected to the PC. */
	virtual bool L0DiscoverNext() = 0;
public:
	virtual ~CommoditiesApi() {};
	/** @brief Obtain a pointer to the first byte of the serial number of the SEcube. The serial number is made of 32 bytes. */
	virtual bool L0DiscoverSerialNo(uint8_t* serialNo) = 0;
};
