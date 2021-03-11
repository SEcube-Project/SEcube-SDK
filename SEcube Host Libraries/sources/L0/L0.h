/**
  ******************************************************************************
  * File Name          : L0.h
  * Description        : Prototypes of the L0 level.
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

/*! \file  L0.h
 *  \brief This header file contains the prototypes of the entire L0 level.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef _L0_H_
#define _L0_H_

#include "Commodities API/commodities_api.h"
#include "Communication API/communication_api.h"
#include "Provision API/provision_api.h"
#include "L0 Base/L0_base.h"

const uint32_t SE3_TIMEOUT = 10000;

class L0 : public CommoditiesApi, public CommunicationApi, public ProvisionApi {
private:
	L0Base base;
	//COMMODITIES
	bool Se3DriveNext() override ;
	bool Se3Info(uint64_t deadline, se3DiscoverInfo* info) override ;
	void L0DiscoverInit() override ;
	bool L0DiscoverNext() override ;
	//COMMUNICATION
	bool Se3Open(uint64_t deadline, se3File* phFile, se3DiscoverInfo* disco) override ;
	uint16_t L0TX(uint16_t cmd, uint16_t cmdFlags, uint16_t len, const uint8_t* data) override ;
	uint16_t L0RX(uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) override ;
	//CLASS ATTRIBUTES
	int nDevices;
public:
	L0();
	~L0();

	//COMMODITIES
	bool L0DiscoverSerialNo(uint8_t* serialNo) override ;

	//COMMUNICATION
	void L0Open() override ;
	void L0Close() override ;
	void L0Open(uint8_t devPtr) override ;
	void L0Close(uint8_t devPtr) override ;
	void L0TXRX(uint16_t reqCmd, uint16_t reqCmdFlags, uint16_t reqLen, const uint8_t* reqData, uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) override ;
	uint16_t L0Echo(const uint8_t* dataIn, uint16_t dataInLen, uint8_t* dataOut) override ;

	//PROVISION
	/** @brief Low level function to initialize the serial number of the SEcube.
	 * @detail Not to be used explicitly. Use corresponding L1FactoryInit() function instead. */
	uint16_t L0FactoryInit(const std::array<uint8_t, L0Communication::Size::SERIAL>& serialno) override ;

	//PERSONAL METHODS
	void L0Restart();
	uint8_t GetNumberDevices();
	bool SwitchToDevice(int devPos);
	uint8_t* GetDeviceHelloMsg();
	se3Char* GetDevicePath(){return this->base.GetDeviceInfoPath();}
	uint8_t* GetDeviceSn(){return this->base.GetDeviceInfoSerialNo();}
	int GetDeviceList(std::vector<std::pair<std::string, std::string>>& devicelist);
	//LOGFILE MANAGING
	bool Se3CreateLogFile(char* path, uint32_t file_dim);
	char* Se3CreateLogFilePath(char *name);
};

#endif
