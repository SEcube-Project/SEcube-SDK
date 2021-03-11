/**
  ******************************************************************************
  * File Name          : login_logout_api.h
  * Description        : L1 functions for the login and logout on the SEcube.
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

/*! \file  login_logout_api.h
 *  \brief This header file defines the L1 functions for the login and logout on the SEcube.
 *  \version SEcube Open Source SDK 1.5.1
 */

#include "../L1 Base/L1_base.h"

class LoginLogoutApi {
public:
	virtual ~LoginLogoutApi() {};
	virtual void L1Login(const std::array<uint8_t, L1Parameters::Size::PIN>& pin, se3_access_type access, bool force) = 0;
	virtual void L1Logout() = 0;
};
