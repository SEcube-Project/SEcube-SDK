/**
  ******************************************************************************
  * File Name          : se3_core_time.c
  * Description        : Firmware internal timer
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

/* WARNING - READ CAREFULLY
 *
 * se3_core_time.c and se3_core_time.h are not used in the firmware.
 * They were used in the past but the code has been completely commented
 * because it is not required anymore. However, the code has been left
 * here since it may still be useful in the future. */

//#include "se3_core_time.h"
//
//uint64_t now;  ///< current UNIX time in seconds
//bool now_initialized;  ///< time was initialized
//int flag = 1;
//
//void se3_time_init(){
//	now_initialized = false;
//	now = 0;
//}
//
//uint64_t se3_time_get()
//{
//#ifdef CUBESIM
//    now = (uint64_t)time(0);
//#endif
//    return now;
//}
//
//void se3_time_set(uint64_t t)
//{
//    now = t;
//	now_initialized = true;
//}
//
//void se3_time_inc()
//{
//    static unsigned int ms = 0;
//    if (++ms == 1000) {
//    	flag = 0;
//        (now)++;
//        ms = 0;
//    }
//}
//
//bool get_now_initialized(){
//	return now_initialized;
//}
