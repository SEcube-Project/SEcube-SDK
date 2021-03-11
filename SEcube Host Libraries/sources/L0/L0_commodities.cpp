/**
  ******************************************************************************
  * File Name          : L0_commodities.cpp
  * Description        : Implementation of the COMMODITY API.
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
 * @file	L0_commodities.cpp
 * @date	July, 2017
 * @brief	implementation of the COMMODITY API
 *
 * The file contains the implementation of the COMMODITY API
 */

#include "L0.h"

///////////////////
//PRIVATE METHODS//
///////////////////
#ifdef _WIN32
//WINDOWS
bool L0::Se3DriveNext() {
	bool found = false;
	while(!found) {
		if (this->base.GetDiscoDrivePath() == NULL) {
			if (this->base.GetDiscoDriveBufLen() == 0){
				return false;
			}
			this->base.SetDiscoDrivePath(this->base.GetDiscoDriveBuf());
		}
		else {
			while (this->base.GetDiscoDrivePath() < this->base.GetDiscoDriveBuf() + this->base.GetDiscoDriveBufLen() && *(this->base.GetDiscoDrivePath()) != L'\0'){
				this->base.SetDiscoDrivePath(this->base.GetDiscoDrivePath() + 1);
			}
			this->base.SetDiscoDrivePath(this->base.GetDiscoDrivePath() + 1);

			if (this->base.GetDiscoDrivePath() >= this->base.GetDiscoDriveBuf() + this->base.GetDiscoDriveBufLen()){
				return false;
			}
		}
		if (L0Support::Se3Win32DiskInDrive(this->base.GetDiscoDrivePath())){
			if (GetDriveTypeW(this->base.GetDiscoDrivePath()) == DRIVE_REMOVABLE){
				found = true;
			}
		}
	}
	return found;
}
#else
//UNIX
bool L0::Se3DriveNext() {
    char buf[SE3_DRIVE_BUF_MAX];
    memset(buf, '\0', SE3_DRIVE_BUF_MAX);
    if(this->base.GetDiscoDriveFile() == nullptr){
    	return false;
    }
    if(fgets(this->base.GetDiscoDriveBuf(), SE3_DRIVE_BUF_MAX, this->base.GetDiscoDriveFile()) != nullptr) {
    	/* this->base.GetDiscoDriveBuf() is se3Char buf[SE3_DRIVE_BUF_MAX + 1] but it is filled by fgets() with SE3_DRIVE_BUF_MAX limit, including NULL terminator character
    	 * buf is char buf[SE3_DRIVE_BUF_MAX], including NULL terminator
    	 * sscanf automatically adds a NULL terminator, which is OK because this->base.GetDiscoDriveBuf() has been initialized by fgets() with SE3_DRIVE_BUF_MAX including NULL terminator, and buf is char buf[SE3_DRIVE_BUF_MAX] */
        sscanf(this->base.GetDiscoDriveBuf(), "%*s%s", buf);
        strcpy(this->base.GetDiscoDriveBuf(), buf);
        this->base.SetDiscoDrivePath(this->base.GetDiscoDriveBuf());
        return true;
    }
    fclose(this->base.GetDiscoDriveFile());
    this->base.SetDiscoDriveFile(nullptr);
    return false;
}
#endif

bool L0::Se3Info(uint64_t deadline, se3DiscoverInfo* info) {
	uint8_t buf[L0Communication::Parameter::COMM_BLOCK];
	se3File hFile;
	int r;

	r = L0Support::Se3OpenExisting(this->base.GetDiscoDrivePath(), false, deadline, &hFile);

	if (r == L0Communication::Error::OK) {
		if (!L0Support::Se3Read(buf, hFile, 15, 1, SE3C_MAGIC_TIMEOUT)) {
			L0Support::Se3Close(hFile);
			return false;
		}

		if (!L0Support::Se3ReadInfo(buf, info)) {
			L0Support::Se3Close(hFile);
			r = L0Communication::Error::ERR_NOT_FOUND;
		}
		else {
			L0Support::Se3Close(hFile);
			return true;
		}
	}

	if (r == L0Communication::Error::ERR_NOT_FOUND) {
		if (L0Support::Se3MagicInit(this->base.GetDiscoDrivePath(), buf, info))
			return true;
		else
			return false;
	}
	//this else can be canceled, since the method returns false anyway
	else
		return false;

	return false;
}

#ifdef _WIN32
//WINDOWS
void L0::L0DiscoverInit() {
	this->base.SetDiscoDriveBufLen(GetLogicalDriveStringsW(SE3_DRIVE_BUF_MAX, this->base.GetDiscoDriveBuf()));
	this->base.SetDiscoDriveBufTermination();
	this->base.SetDiscoDrivePath(NULL);
}
#else
//UNIX
void L0::L0DiscoverInit() {
	this->base.SetDiscoDriveFile(fopen("/proc/mounts", "r"));
}
#endif

bool L0::L0DiscoverNext() {
	se3DiscoverInfo discovNfo;
	uint64_t deadline;

	while(Se3DriveNext()) {
		deadline = L0Support::Se3Deadline(0);
		if (Se3Info(deadline, &discovNfo)) {
			memcpy(this->base.GetDiscoDeviceSerialNo(), discovNfo.serialno, L0Communication::Size::SERIAL);
			memcpy(this->base.GetDiscoDeviceHelloMsg(), discovNfo.hello_msg, L0Communication::Size::HELLO);
			L0Support::Se3PathCopy(this->base.GetDiscoDevicePath(), this->base.GetDiscoDrivePath());
			this->base.SetDiscoDeviceStatus(discovNfo.status);

			return true;
		}
	}
	return false;
}

//////////////////
//PUBLIC METHODS//
//////////////////

bool L0::L0DiscoverSerialNo(uint8_t* serialNo) {
	L0DiscoverInit();
	while(L0DiscoverNext()) {
		if (memcmp(serialNo, this->base.GetDiscoDeviceSerialNo(), L0Communication::Size::SERIAL))
			return true;
	}

	return false;
}
