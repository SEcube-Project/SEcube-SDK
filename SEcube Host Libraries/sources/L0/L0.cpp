/**
  ******************************************************************************
  * File Name          : L0.cpp
  * Description        : Implementation of the L0 methods.
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
 * @file	L0.cpp
 * @date	July, 2017
 * @brief	Implementation of the L0 methods
 *
 * The file contains the implementation of the methods that belong directly to the L0 LIBRARY (doesn't include the implementation of the APIs)
 */

#include "L0.h"
#include <string>

int L0::GetDeviceList(std::vector<std::pair<std::string, std::string>>& devicelist){
    std::string devicepath;
    this->L0Restart();
	this->L0DiscoverInit();
    while(this->L0DiscoverNext()){
#if defined(__linux__) || defined(__APPLE__)
		char *c = (char*)this->base.GetDiscoDevicePath();
		devicepath = std::string(c);
		if(devicepath.back() != '/'){
			devicepath.push_back('/'); // add slash if required
		}
		std::string sn((char*)this->base.GetDiscoDeviceSerialNo(), L0Communication::Size::SERIAL);
#elif _WIN32
		size_t retval;
		char path[L0Communication::Parameter::SE3_MAX_PATH];
		memset(path, '\0', L0Communication::Parameter::SE3_MAX_PATH);
		if(wcstombs_s(&retval, path, L0Communication::Parameter::SE3_MAX_PATH, this->base.GetDiscoDevicePath(), L0Communication::Parameter::SE3_MAX_PATH-1) == 0){ // convert from wchar_t
			devicepath = std::string(path);
			if(devicepath.back() != '\\'){
				devicepath.push_back('\\'); // add backslash if required
			}
		} else {
			return -1;
		}
		std::string sn((char*)this->base.GetDiscoDeviceSerialNo(), L0Communication::Size::SERIAL);
#endif
		std::pair<std::string, std::string> p(devicepath, sn);
		devicelist.push_back(p);
    }
    return 0;
}

L0::L0() {
	//initialize the secube discover
	L0DiscoverInit();
	//scan all the seCubes connected
	while(L0DiscoverNext()){
		//adding the device parses all the iterator data inside the device
		this->base.AddDevice();
	}
	this->nDevices = this->base.GetNDevices();
	if(this->nDevices > 0){ // set all devices by default to closed
		int originalptr = this->base.GetDevicePtr();
		for(int i = 0; i < this->nDevices; i++){
			this->base.SetDevicePtr(i);
			this->base.SetDeviceOpened(false);
		}
		this->base.SetDevicePtr(originalptr); // go back to first device
	}
// cambiare qui per creare il file di log
#ifdef SE3_LOG_FILE
	//Se3CreateLogFilePath("log_file.txt")
	this->nDevices = this->base.GetNDevices();
	//if ( ! Se3CreateLogFile("/media/nico/6163-3831/log_file.txt",2) )  // cambiare qui
		//printf("log file not created\n");
#endif
}

L0::~L0() {
	//clear the array containing all the devices
	this->base.ResetDeviceArray();
}

void L0::L0Restart() {
	//close all the devices
	for (uint8_t i = 0; i < this->nDevices; i++)
		L0Close(i);
	//clear all the secube devices found
	this->base.ResetDeviceArray();

	//re-initialise the devices as done in the ctor
	L0DiscoverInit();

	while(L0DiscoverNext()){
		this->base.AddDevice();
	}

	this->nDevices = this->base.GetNDevices();
}

uint8_t L0::GetNumberDevices() {
	return this->nDevices;
}

bool L0::SwitchToDevice(int devPos) {
	return this->base.SetDevicePtr(devPos);
}

uint8_t* L0::GetDeviceHelloMsg() {
	return this->base.GetDeviceHelloMsg();
}
