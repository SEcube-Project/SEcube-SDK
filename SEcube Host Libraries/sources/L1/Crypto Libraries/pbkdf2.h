/**
  ******************************************************************************
  * File Name          : pbkdf2.h
  * Description        : Low level PBKDF2 header file.
  ******************************************************************************
  *
  * Copyright © 2016-present Blu5 Group <https://www.blu5group.com>
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

/*! \file  pbkdf2.h
 *  \brief This file defines what is needed for the PBKDF2HmacSha256 algorithm. These functions are used exclusively to implement very low-level features in the SEcube SDK.
 *  \version SEcube Open Source SDK 1.5.1
 *  \detail This header file, together with the actual implementation, is provided by Blu5 (https://www.blu5group.com/).
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "sha256.h"

void PBKDF2HmacSha256(	const uint8_t *pw,
						size_t npw,
						const uint8_t *salt,
						size_t nsalt,
						uint32_t iterations,
						uint8_t *out,
						size_t nout);

#ifdef __cplusplus
}
#endif
