/**
  ******************************************************************************
  * File Name          : pbkdf2.h
  * Description        : This file contains defines and functions for PBDKDF2
  *                      (Password-Based Key Derivation Function 2)
  ******************************************************************************
  *
  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
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

#pragma once

#include <stdint.h>
#include <string.h>

#include "sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 *  \brief Public Key Derivation Function 2
	 *  
	 *  \param [in] pw Master password
	 *  \param [in] npw Length of Master password
	 *  \param [in] salt Salt
	 *  \param [in] nsalt Length of Salt
	 *  \param [in] iterations Number of iterations
	 *  \param [in] out Pointer to output
	 *  \param [in] nout Length of ouput
	 *  
	 */
	void PBKDF2HmacSha256(
		const uint8_t *pw, size_t npw,
		const uint8_t *salt, size_t nsalt,
		uint32_t iterations,
		uint8_t *out, size_t nout);

#ifdef __cplusplus
}
#endif
