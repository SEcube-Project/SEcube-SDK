/**
  ******************************************************************************
  * File Name          : L1.cpp
  * Description        : Implementation of the L1 methods.
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
 * @file	L1.cpp
 * @date	July, 2017
 * @brief	Implementation of the L1 methods
 *
 * The file contains the implementation of the methods that belong directly to the L1 LIBRARY (doesn't include the implementation of the APIs)
 */

#include "L1.h"

using namespace std;

void L1::SessionInit() {
	this->base.InitializeSession(this->GetNumberDevices());
}

void SEcube_ciphertext::reset(){
	this->algorithm = L1Algorithms::Algorithms::ALGORITHM_MAX;
	this->ciphertext_size = 0;
	this->key_id = 0;
	this->mode = CryptoInitialisation::Modes::INVALID_AES_MODE;
	this->digest.fill(0);
	this->initialization_vector.fill(0);
	this->digest_nonce.fill(0);
	this->CTR_nonce.fill(0);
	this->ciphertext.reset();
}

L1::L1() { // default constructor
	this->SessionInit();
	uint8_t numdevices = this->GetNumberDevices();
	for(int i = (int)numdevices - 1; i >= 0; i--){ // open in reverse order so that the default SEcube is the number 0 in the list (low level L0 list)
		this->L0Open(i);
	}
	this->index = 255; // never used, except by SEkey initialization API for the SEcube of the users
}

void L1::L1SelectSEcube(array<uint8_t, L0Communication::Size::SERIAL>& sn){
	uint8_t indx = 0;
	for(uint8_t i = 0; i < this->GetNumberDevices(); i++){
		if(memcmp(this->GetDeviceSn(), sn.data(), L0Communication::Size::SERIAL) == 0){
			indx = i;
			break;
		}
	}
	L1SelectDeviceException selectDevExc;
	if (!this->SwitchToDevice(indx)){
		throw selectDevExc;
	}
	this->base.SwitchToSession(indx);
}

void L1::L1SelectSEcube(uint8_t indx){
	L1SelectDeviceException selectDevExc;
	if (!this->SwitchToDevice(indx)){
		throw selectDevExc;
	}
	this->base.SwitchToSession(indx);
}

L1::~L1() {
	if(this->index != 255){ // this is used by SEkey
		if (this->base.GetSessionLoggedIn()){
			L1Logout();
		}
		this->L0Close(this->index); // added to remove memory leakage of se3file
	} else { // this is used always except for a particular case of SEkey
		for (uint8_t i = 0; i < this->GetNumberDevices(); i++) {
			this->base.SwitchToSession(i);
			if (this->base.GetSessionLoggedIn()){
				L1Logout();
			}
			this->L0Close(i); // added to remove memory leakage of se3file
		}
	}
}

void L1::PrepareSessionBufferForChallenge(uint8_t* cc1, uint8_t* cc2, uint16_t access) {
	//get a byte pointer for the 2 bytes variable (access)
	uint8_t* _access = (uint8_t*)&access;

	this->base.FillSessionBuffer(	cc1,
									L1Response::Offset::DATA + L1ChallengeRequest::Offset::CC1,
									L1Parameters::Size::CHALLENGE);
	this->base.FillSessionBuffer(	cc2,
									L1Response::Offset::DATA + L1ChallengeRequest::Offset::CC2,
									L1Parameters::Size::CHALLENGE);
	this->base.FillSessionBuffer(	_access,
									L1Response::Offset::DATA + L1ChallengeRequest::Offset::ACCESS,
									2);
}

//public

void L1::TXRXData(uint16_t cmd, uint16_t reqLen, uint16_t cmdFlags, uint16_t* respLen) {
	//SET THE HEADERS
	if (this->base.GetSessionLoggedIn()){ // fill the buffer with the token
		this->base.FillSessionBuffer(this->base.GetSessionToken(), L1Request::Offset::TOKEN, L1Parameters::Size::TOKEN);
	} else {
		this->base.FillSessionBuffer(L1Request::Offset::TOKEN, L1Parameters::Size::TOKEN); // fill the buffer with 0s
	}

	uint8_t* _cmd = (uint8_t*)&cmd;
	uint8_t* _reqLen = (uint8_t*)&reqLen;

	uint8_t* reqIv = this->base.GetSessionBuffer() + L1Request::Offset::IV;

	this->base.FillSessionBuffer(_cmd, L1Request::Offset::CMD, 2);
	this->base.FillSessionBuffer(_reqLen, L1Request::Offset::LEN, 2);

	uint16_t reqLenPadded = reqLen;
	if (reqLenPadded % L1Parameters::Size::CRYPTO_BLOCK != 0) {
		//fill the buffer with 0s in the request length pat
		this->base.FillSessionBuffer(	reqLenPadded + L1Response::Offset::DATA,
										L1Parameters::Size::CRYPTO_BLOCK - (reqLenPadded % L1Parameters::Size::CRYPTO_BLOCK));
										reqLenPadded += L1Parameters::Size::CRYPTO_BLOCK - (reqLenPadded % L1Parameters::Size::CRYPTO_BLOCK);
	}

	//ENCRYPT
	//check if the session is not initialized
	if (!this->base.GetSessionCryptoInitialized()) {
		//if not initialize it
		Se3PayloadCryptoInit();
		this->base.SetCryptoctxInizialized(true);
	}

	if (cmdFlags & L1Commands::Flags::ENCRYPT){
		L0Support::Se3Rand(L1Parameters::Size::CRYPTO_BLOCK, reqIv);
	} else {
		this->base.FillSessionBuffer(L1Request::Offset::IV, L1Parameters::Size::CRYPTO_BLOCK);
	}

	uint16_t req0Len = L1Request::Offset::DATA + reqLenPadded;
	uint8_t* reqAuth = this->base.GetSessionBuffer() + L1Request::Offset::AUTH;

	Se3PayloadEncrypt(	cmdFlags,
						this->base.GetSessionBuffer() + L1Request::Offset::IV,
						this->base.GetSessionBuffer() + L1Parameters::Size::AUTH + L1Parameters::Size::IV,
						(req0Len - L1Parameters::Size::AUTH - L1Parameters::Size::IV) / L1Parameters::Size::CRYPTO_BLOCK,
						reqAuth);

	uint16_t resp0Len = L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK;

	uint16_t respStatus;
	L1TXRXException commExc;
	bool dataSent = false;

	while(!dataSent) {
		try {
			L0::L0TXRX(L0Commands::Command::L1_CMD0, cmdFlags, req0Len, this->base.GetSessionBuffer(), &respStatus, &resp0Len, this->base.GetSessionBuffer());
			dataSent = true;
		}
		catch(const L0NoDeviceOpenedException& e) {
//			printf("The device was closed!!!\n");
			L0Open();
		}
		catch (const std::exception& e){
			cout << e.what() << endl;
			throw commExc;
		}
		catch(...) {
//			printf("Other exception\n");
			throw commExc;
		}

        if(respStatus == L1Error::Error::SE3_ERR_OPENED)
        {
            L1AlreadyOpenException alreadyOpenExc;
            throw alreadyOpenExc;
        }
        else if(respStatus != L1Error::Error::OK)
        {
            cout << "[L1.cpp - L1::TXRXData] Debug: Response status from L0::L0TXRX -> " << respStatus <<endl;
            L0TXRXException l0TxRxExc;
            throw l0TxRxExc;

        }
	}

	//DECRYPT
	uint8_t* respIv = this->base.GetSessionBuffer() + L1Response::Offset::IV;
	uint8_t* resp_auth = this->base.GetSessionBuffer() + L1Response::Offset::AUTH;
	L1CommunicationError payloadDecryptExc;

	try {
	Se3PayloadDecrypt(	cmdFlags,
						respIv,
						this->base.GetSessionBuffer() + L1Parameters::Size::AUTH + L1Parameters::Size::IV,
						(resp0Len - L1Parameters::Size::AUTH - L1Parameters::Size::IV) / L1Parameters::Size::CRYPTO_BLOCK,
						resp_auth);
	}
	catch (L1Exception& e) {
		throw payloadDecryptExc;
	}

	uint16_t u16tmp;

	memcpy((void*)&u16tmp, (const void*)(this->base.GetSessionBuffer() + L1Response::Offset::LEN), 2);
	*respLen = u16tmp;
	memcpy((void*)&u16tmp, (const void*)(this->base.GetSessionBuffer() + L1Response::Offset::STATUS), 2);

	if (u16tmp != L0ErrorCodes::Error::OK)
		throw commExc;
}

void L1::Se3PayloadCryptoInit() {
	uint8_t keys[2 * B5_AES_256];
	PBKDF2HmacSha256(this->base.GetSessionKey(), B5_AES_256, NULL, 0, 1, keys, 2 * B5_AES_256);
	B5_Aes256_Init(this->base.GetSessionCryptoctxAesenc(), keys, B5_AES_256, B5_AES256_CBC_ENC);
	B5_Aes256_Init(this->base.GetSessionCryptoctxAesdec(), keys, B5_AES_256, B5_AES256_CBC_DEC);
	this->base.SetSessionCryptoctxHmacKey(keys + B5_AES_256, 0, B5_AES_256);
}

void L1::Se3PayloadEncrypt(uint16_t flags, uint8_t* iv, uint8_t* data, uint16_t nBlocks, uint8_t* auth) {

    if (flags & L1Commands::Flags::ENCRYPT) {
        B5_Aes256_SetIV(this->base.GetSessionCryptoctxAesenc(), iv);
        B5_Aes256_Update(this->base.GetSessionCryptoctxAesenc(), data, data, nBlocks);
    }

    if (flags & L1Commands::Flags::SIGN) {
        B5_HmacSha256_Init(this->base.GetSessionCryptoctxHmac(), this->base.GetSessionCryptoctxHmacKey(), B5_AES_256);
        B5_HmacSha256_Update(this->base.GetSessionCryptoctxHmac(), iv, B5_AES_IV_SIZE);
        B5_HmacSha256_Update(this->base.GetSessionCryptoctxHmac(), data, nBlocks * B5_AES_BLK_SIZE);
        B5_HmacSha256_Finit(this->base.GetSessionCryptoctxHmac(), this->base.GetSessionCryptoctxAuth());
        memcpy(auth, this->base.GetSessionCryptoctxAuth(), 16);
    }
    else {
        memset(auth, 0, 16);
    }
}

void L1::Se3PayloadDecrypt(uint16_t flags, const uint8_t* iv, uint8_t* data, uint16_t nBlocks, const uint8_t* auth) {
	L1PayloadDecryptionException PayloadDecExc;

    if (flags & L1Commands::Flags::SIGN) {
        B5_HmacSha256_Init(this->base.GetSessionCryptoctxHmac(), this->base.GetSessionCryptoctxHmacKey(), B5_AES_256);
        B5_HmacSha256_Update(this->base.GetSessionCryptoctxHmac(), iv, B5_AES_IV_SIZE);
        B5_HmacSha256_Update(this->base.GetSessionCryptoctxHmac(), data, nBlocks * B5_AES_BLK_SIZE);
        B5_HmacSha256_Finit(this->base.GetSessionCryptoctxHmac(), this->base.GetSessionCryptoctxAuth());
        if (memcmp(auth, this->base.GetSessionCryptoctxAuth(), 16)) {
            throw PayloadDecExc;
        }
    }

    if (flags & L1Commands::Flags::ENCRYPT) {
        B5_Aes256_SetIV(this->base.GetSessionCryptoctxAesdec(), iv);
        B5_Aes256_Update(this->base.GetSessionCryptoctxAesdec(), data, data, nBlocks);
    }
}

void L1::L1Config(uint16_t type, uint16_t op, std::array<uint8_t, L1Parameters::Size::PIN>& value) {
	this->base.FillSessionBuffer((uint8_t*)&type, L1Response::Offset::DATA + L1Configuration::RequestOffset::CONFIG_ID,	2);
	this->base.FillSessionBuffer((uint8_t*)&op,	L1Response::Offset::DATA + L1Configuration::RequestOffset::CONFIG_OP, 2);
	if(op == L1Configuration::Operation::GET){
		this->base.FillSessionBuffer(L1Response::Offset::DATA + L1Configuration::RequestOffset::CONFIG_VALUE, L1Configuration::RecordSize::RECORD_SIZE);
	}
	if(op == L1Configuration::Operation::SET){
		this->base.FillSessionBuffer(value.data(), L1Response::Offset::DATA + L1Configuration::RequestOffset::CONFIG_VALUE, L1Parameters::Size::PIN);
	}
	uint16_t dataLen = 2 + 2 + L1Configuration::RecordSize::RECORD_SIZE;
	uint16_t respLen = 0;
	L1ConfigException configExc;
	try {
		TXRXData(L1Commands::Codes::CONFIG, dataLen, 0, &respLen);
	}
	catch(L1Exception& e) {
		throw configExc;
	}
	if(op == L1Configuration::Operation::GET){ //read the response
		this->base.ReadSessionBuffer(value.data(),	L1Response::Offset::DATA + L1Configuration::ResponseOffset::CONFIG_VALUE, L1Configuration::RecordSize::RECORD_SIZE);
	}
}

void L1::L1FactoryInit(const std::array<uint8_t, L0Communication::Size::SERIAL>& serialno) {
	DeviceAlreadyInitializedException exA;
	L0FactoryInitException exB;
	uint16_t r = this->L0FactoryInit(serialno);
	if(r == L0ErrorCodes::Error::SE3_ERR_STATE){
		throw exA;
	} else {
		if(r != L0ErrorCodes::Error::OK){
			throw exB;
		}
	}
}

void L1::GetDeviceSerialNumber(string& sn){
	char *buf = (char*)this->GetDeviceSn();
	sn = string(buf, L0Communication::Size::SERIAL);
}

bool L1::L1GetSessionLoggedIn(){
	return this->base.GetSessionLoggedIn();
}

se3_access_type L1::L1GetAccessType(){
	return this->base.GetSessionAccessType();
}
