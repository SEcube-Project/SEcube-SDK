/**
  ******************************************************************************
  * File Name          : L0_communication.cpp
  * Description        : Implementation of the COMMUNICATION API.
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
 * @file	L0_communication.cpp
 * @date	July, 2017
 * @brief	Implementation of the COMMUNICATION API
 *
 * The file contains the implementation of the COMMUNICATION API
 */

#include "L0.h"
#include "L0_error_manager.h"

using namespace std;

///////////////////
//PRIVATE METHODS//
///////////////////

bool L0::Se3Open(uint64_t deadline, se3File* phFile, se3DiscoverInfo* disco) {
	uint8_t buf[L0Communication::Parameter::COMM_BLOCK];
	se3File hFile;
	int r;
	bool discoverInfoRead = false;

	r = L0Support::Se3OpenExisting(this->base.GetDeviceInfoPath(), true, deadline, &hFile);

	if (r == L0Communication::Error::ERR_NOT_FOUND) {
		L0Support::Se3MagicInit(this->base.GetDeviceInfoPath(), buf, disco);
		discoverInfoRead = true;
		// try again regardless of result. another process may have created the magic file
		r = L0Support::Se3OpenExisting(this->base.GetDeviceInfoPath(), true, deadline, &hFile);
	}

	if (r != L0Communication::Error::OK)
		return false;

	if (!discoverInfoRead) {
		if (!L0Support::Se3Read(buf, hFile, 15, 1, SE3C_MAGIC_TIMEOUT)) {
			L0Support::Se3Close(hFile);
			return false;
		}
		if (!L0Support::Se3ReadInfo(buf, disco)) {
			//not a SEcube
			L0Support::Se3Close(hFile);
			return false;
		}
	}

	*phFile = hFile;
	return true;
}

uint16_t L0::L0TX(uint16_t cmd, uint16_t cmdFlags, uint16_t len, const uint8_t* data) {
#if SE3_CONF_CRC
	uint16_t crc;
#endif

	uint32_t cmdToken = 0;				//Command Token
	uint16_t lenDataAndHeaders = L0Support::Se3ReqLenDataAndHeaders(len);
	uint16_t n = 0;
	uint32_t offsetDst = 0;				//Offset for destination blocks buffer
	uint32_t offsetSrc = 0;				//Offset for source data buffer
	uint16_t nBlocks = 0;				//Number of logical data blocks

	L0Support::Se3Rand(sizeof(uint32_t), (uint8_t*)&cmdToken);

	/* Set header fields */
	SE3SET16(this->base.GetDeviceRequest(), L0Request::Offset::CMD, cmd);
	SE3SET16(this->base.GetDeviceRequest(), L0Request::Offset::CMD_FLAGS, cmdFlags);
	SE3SET16(this->base.GetDeviceRequest(), L0Request::Offset::LEN, lenDataAndHeaders);
	SE3SET32(this->base.GetDeviceRequest(), L0Request::Offset::CMD_TOKEN, cmdToken);
	memset(this->base.GetDeviceRequest() + L0Request::Offset::PADDING, 0, 4);

#if SE3_CONF_CRC
	// compute crc of headers and data
	crc = Se3Crc16Update(L0Request::Offset::CRC, this->base.GetDeviceRequest(), 0);
	if (len > 0) {
		crc = Se3Crc16Update(len, data, crc);
	}
	SE3SET16(this->base.GetDeviceRequest(), L0Request::Offset::CRC, crc);
#endif

	//set the data
	n =	len < L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER ?
		len :
		L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER;
	memcpy(this->base.GetDeviceRequest() + L0Request::Size::HEADER, data, n);
	offsetDst += L0Communication::Parameter::COMM_BLOCK;
	offsetSrc += n;
	nBlocks++;

	while (offsetSrc < len) {
		cmdToken++;
		n =	len - offsetSrc < L0Communication::Parameter::COMM_BLOCK - L0Request::Size::DATA_HEADER ?
			len - offsetSrc :
			L0Communication::Parameter::COMM_BLOCK - L0Request::Size::DATA_HEADER;
		SE3SET32(this->base.GetDeviceRequest() + offsetDst, L0Request::Offset::DATA_CMD_TOKEN, cmdToken);
		// @matteo: changed L0Request::Offset::DATA to L0Request::Offset::SE3_REQDATA_OFFSET_DATA in next line
		memcpy(this->base.GetDeviceRequest() + offsetDst + L0Request::Offset::SE3_REQDATA_OFFSET_DATA, data + offsetSrc, n);
		offsetDst += L0Communication::Parameter::COMM_BLOCK;
		offsetSrc += n;
		nBlocks++;
	}

	//send the data by writing inside the file
	if (!L0Support::Se3Write(this->base.GetDeviceRequest(), this->base.GetDeviceFile(), 0, nBlocks, SE3_TIMEOUT))
		return L0ErrorCodes::Error::COMMUNICATION;

	return L0ErrorCodes::Error::OK;
}

uint16_t L0::L0RX(uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) {
#if SE3_CONF_CRC
	uint16_t crc
#endif

	bool ready = false;
	bool success = true;
	uint16_t u16tmp;
	uint64_t deadline = L0Support::Se3Deadline(SE3_TIMEOUT);
	uint16_t lenDataAndHeaders = 0;
	uint16_t len = 0;
	size_t nBlocks = 0;
	uint32_t cmdtok0;
	size_t i = 0;
	uint32_t u32tmp;
	uint16_t n;
	uint16_t offsetSrc;
	uint16_t offsetDst;

	while (!ready) {
		Se3Sleep();

		if (!L0Support::Se3Read(this->base.GetDeviceResponse(), this->base.GetDeviceFile(), 0, 1, SE3_TIMEOUT)) {
			success = false;
			break;
		}

		SE3GET16(this->base.GetDeviceResponse(), 0, u16tmp);
		ready = u16tmp == 1;

		if (L0Support::Se3Clock() > deadline && !ready) {
			success = false;
			break;
		}
	}

	if (!success)
		return L0ErrorCodes::Error::COMMUNICATION;

	SE3GET16(this->base.GetDeviceResponse(), L0Response::Offset::LEN, lenDataAndHeaders);
	len = L0Support::Se3RespLenData(lenDataAndHeaders);

	if (len > *respLen)
		return L0ErrorCodes::Error::COMMUNICATION;

	nBlocks = L0Support::Se3NBlocks(lenDataAndHeaders);

	if (nBlocks > 1)
		if (!L0Support::Se3Read(this->base.GetDeviceResponse() + L0Communication::Parameter::COMM_BLOCK, this->base.GetDeviceFile(), 1, nBlocks - 1, SE3_TIMEOUT))
			return L0ErrorCodes::Error::COMMUNICATION;

	//check cmdtokens
	SE3GET32(this->base.GetDeviceResponse(), L0Response::Offset::CMD_TOKEN, cmdtok0);

	for (i = 1; i < nBlocks; i++) {
		cmdtok0++;
		SE3GET32(this->base.GetDeviceResponse() + i * L0Communication::Parameter::COMM_BLOCK, L0Response::Offset::DATA_CMD_TOKEN, u32tmp);
		if (cmdtok0 != u32tmp)
			return L0ErrorCodes::Error::COMMUNICATION;
	}

#if SE3_CONF_CRC
	crc = Se3Crc16Update(SE3_REQ_OFFSET_CRC, this->base.GetDeviceResponse(), 0);
#endif

	// @matteo: this is the original C++ implementation which is wrong
	// n = len < L0Communication::Parameter::COMM_BLOCK - L0Response::Size::HEADER ? len :	L0Communication::Parameter::COMM_BLOCK - L0Response::Size::DATA_HEADER;

	// @matteo: this is the correct implementation (copied from C version)
	n = (len < (L0Communication::Parameter::COMM_BLOCK - L0Response::Size::HEADER)) ? (len) : (L0Communication::Parameter::COMM_BLOCK - L0Response::Size::HEADER);

	if (respData != NULL)
		memcpy(respData, this->base.GetDeviceResponse() + L0Response::Size::HEADER, n);

#if SE3_CONF_CRC
	if (n > 0)
		crc = Se3Crc16Update(n, this->base.GetDeviceResponse() + SE3_RESP_SIZE_HEADER, crc);
#endif

	offsetSrc = L0Communication::Parameter::COMM_BLOCK;
	offsetDst = n;

	while (offsetDst < len) {
		n =	len - offsetDst < L0Communication::Parameter::COMM_BLOCK - L0Response::Size::DATA_HEADER ?
			len - offsetDst :
			L0Communication::Parameter::COMM_BLOCK - L0Response::Size::DATA_HEADER;
		if (respData != NULL)
			memcpy(respData + offsetDst, this->base.GetDeviceResponse() + offsetSrc + L0Response::Size::DATA_HEADER, n);

#if SE3_CONF_CRC
		crc = Se3Crc16Update(n, this->base.GetDeviceResponse() + offsetSrc + SE3_RESPDATA_SIZE_HEADER, crc);
#endif

		offsetSrc += L0Communication::Parameter::COMM_BLOCK;
		offsetDst += n;
	}

	//read headers
	SE3GET16(this->base.GetDeviceResponse(), L0Response::Offset::STATUS, u16tmp);
	*respStatus = u16tmp;
	SE3GET16(this->base.GetDeviceResponse(), L0Response::Offset::LEN, u16tmp);
	*respLen = len;

#if SE3_CONF_CRC
	SE3GET16(this->base.GetDeviceResponse(), SE3_RESP_OFFSET_CRC, u16tmp);

	if (u16tmp != crc)
		return SE3_ERR_COMM;
#endif

	return L0ErrorCodes::Error::OK;
}

//////////////////
//PUBLIC METHODS//
//////////////////

void L0::L0Open() {
	L0Open(this->base.GetDevicePtr());
}

void L0::L0Open(uint8_t devPtr) {
	se3File hFile;
	se3DiscoverInfo discovNfo;
	L0NoDeviceException ptrExc;
	L0CommunicationErrorException commExc;

	if (this->base.GetDeviceOpened()){
		return;
	}

	// check if the wanted device exists
	if (!this->base.SetDevicePtr(devPtr)){
		throw ptrExc;
	}

	// set the opened flag to false inside the currently selected device
	this->base.SetDeviceOpened(false);

	if(!Se3Open(SE3_TIMEOUT, &hFile, &discovNfo)){
		throw commExc;
	}

	if (memcmp(discovNfo.serialno, this->base.GetDeviceInfoSerialNo(), L0Communication::Size::SERIAL)) {
		L0Support::Se3Close(hFile);
		throw commExc;
	}

	//set the file handler inside the currently selected device
	this->base.SetDeviceFile(hFile);
	//allocate the memory for the request buffer (transmission)
	this->base.AllocateDeviceRequest();
	//allocate the memory for the response buffer (reception)
	this->base.AllocateDeviceResponse();
	//set the opened flag inside the currently selected device
	this->base.SetDeviceOpened(true);
}

void L0::L0Close() {
	L0Close(this->base.GetDevicePtr());
}

void L0::L0Close(uint8_t devPtr) {
	L0NoDeviceException ptrExc;

	if (!this->base.SetDevicePtr(devPtr))
		throw ptrExc;

	if (this->base.GetDeviceOpened()) {
		this->base.SetDeviceOpened(false);
		if (this->base.GetDeviceRequest() != NULL){
			this->base.FreeDeviceRequest();
		}
		if (this->base.GetDeviceResponse() == NULL){
			this->base.FreeDeviceResponse();
		}
		L0Support::Se3Close(this->base.GetDeviceFile());
	}
}

void L0::L0TXRX(uint16_t reqCmd, uint16_t reqCmdFlags, uint16_t reqLen, const uint8_t* reqData, uint16_t* respStatus, uint16_t* respLen, uint8_t* respData) {
	uint16_t error = 0;	//error value

	L0NoDeviceOpenedException noDevExc;
	L0ParametersErrorException paramEcx;
	L0TXException txExc;
	L0RXException rxExc;

	if (!this->base.GetDeviceOpened())
		throw noDevExc;

	if (reqLen > L0Request::Size::MAX_DATA)
		throw paramEcx;

	//if (this->base.GetDevice() == NULL || reqLen > SE3_REQ_MAX_DATA)
		//return SE3_ERR_PARAMS;

	error = L0TX(reqCmd, reqCmdFlags, reqLen, reqData);

	if (error != L0ErrorCodes::Error::OK)
		throw txExc;

	error = L0RX(respStatus, respLen, respData);

	if (error != L0ErrorCodes::Error::OK)
		throw rxExc;
}

uint16_t L0::L0Echo(const uint8_t* dataIn, uint16_t dataInLen, uint8_t* dataOut) {
	uint16_t respStatus = 0;
	uint16_t respLen = 0;
	L0EchoException echoExc;
	respLen = dataInLen;
	try {
		L0TXRX(L0Commands::Command::ECHO, 0, dataInLen, dataIn, &respStatus, &respLen, dataOut);
	}
	catch (L0Exception& e) {
		throw echoExc;
	}
	return respStatus;
}
