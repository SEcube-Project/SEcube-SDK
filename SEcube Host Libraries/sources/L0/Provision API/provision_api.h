/**
  ******************************************************************************
  * File Name          : provision_api.h
  * Description        : Prototypes of the PROVISION API.
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

/*! \file  provision_api.h
 *  \brief Prototypes of the PROVISION API.
 *  \version SEcube Open Source SDK 1.5.1
 */

class ProvisionApi {
public:
	virtual ~ProvisionApi() {};
	virtual uint16_t L0FactoryInit(const std::array<uint8_t, L0Communication::Size::SERIAL>& serialno) = 0;
};
