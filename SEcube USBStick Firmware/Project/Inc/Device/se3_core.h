/**
  ******************************************************************************
  * File Name          : se3_core.h
  * Description        : Firmware core functionalities
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


#include <se3c0def.h>


#if defined(_MSC_VER)
#define SE3_ALIGN_16 __declspec(align(0x10))
#elif defined(__GNUC__)
#define SE3_ALIGN_16 __attribute__((aligned(0x10)))
#else
#define SE3_ALIGN_16
#endif



/** \brief Initialise the device modules
 *
 * Initialise the main cores and data structures
 */
void device_init();


/** \brief Endless loop that executes the commands
 *
 * 	This function stays in idle waiting for command and data transfer requests
 */
void device_loop();


/** \brief Execute received command
 *
 *  Process the last received request and produce a response
 */
void se3_cmd_execute();


/** \brief ECHO command handler
 *
 *  Send back received data
 */
uint16_t echo(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief FACTORY_INIT command handler
 *
 *  Initialise the serial number of the SEcube.
 */
uint16_t factory_init(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);


/** \brief FACTORY_INIT command handler
 *
 *  Reset USEcube to boot mode
 */
uint16_t bootmode_reset(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);
