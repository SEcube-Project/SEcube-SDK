/**
  ******************************************************************************
  * File Name          : utility_api.h
  * Description        : Prototypes of the utility API.
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

/*! \file  utility_api.h
 *  \brief Prototypes of the utility API.
 *  \version SEcube Open Source SDK 1.5.1
 *  \detail This header simply defines some functions that must be implemented by L1.
 */

#ifndef UTILITY_API_H_
#define UTILITY_API_H_

#include "../L1 Base/L1_base.h"

class UtilityApi {
public:
	virtual ~UtilityApi() {};
	virtual void L1SetAdminPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) = 0;
	virtual void L1SetUserPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) = 0;
};

#endif
