/**
  ******************************************************************************
  * File Name          : L1_base.cpp
  * Description        : Implementation of the L1Base library.
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
 * @file	L1_base.cpp
 * @date	July, 2017
 * @brief	Implementation of the L1Base library
 *
 * The file contains all the implementation of the L1Base library
 */

#include "L1_base.h"

L1Base::L1Base() {
	this->ptr = 0;
}

L1Base::~L1Base() {
	s.clear();
}

void L1Base::InitializeSession(uint8_t nSessions) {
	for (size_t i = 0; i < nSessions; i++) {
		se3Session _s;
		memset(&_s, 0, sizeof(se3Session));
		memcpy(_s.key, se3Magic, L1Parameters::Size::KEY);
		_s.cryptoctx_initialized = false;
		//add the session to the list
		this->s.push_back(_s);
	}
}

void L1Base::SwitchToSession(uint8_t sPtr) {
	this->ptr = sPtr;
}

void L1Base::FillSessionBuffer(uint8_t* data, size_t offset, size_t len) {
	memcpy(this->s[this->ptr].buf + offset, data, len);
}

void L1Base::FillSessionBuffer(size_t offset, size_t len) {
	for (size_t i = offset; i < offset + len; i++)
		this->s[this->ptr].buf[i] = 0;
}

uint8_t* L1Base::GetSessionBuffer() {
	return this->s[this->ptr].buf;
}

bool L1Base::GetSessionLoggedIn() {
	return this->s[this->ptr].logged_in;
}

void L1Base::SetSessionLoggedIn(bool logged) {
	this->s[this->ptr].logged_in = logged;
}

se3_access_type L1Base::GetSessionAccessType(){
	return this->s[this->ptr].access_type;
}

void L1Base::SetSessionAccessType(se3_access_type access) {
	this->s[this->ptr].access_type = access;
}

bool L1Base::GetSessionCryptoInitialized() {
	return this->s[this->ptr].cryptoctx_initialized;
}

uint8_t* L1Base::GetSessionToken() {
	return this->s[this->ptr].token;
}

void L1Base::SetSessionToken(size_t offset, size_t len) {
	L1OutOfBoundsException boundExc;

	if (offset + len > L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK)
		throw boundExc;

	for(size_t i = 0; i < len; i++)
		this->s[this->ptr].token[i] = this->s[this->ptr].buf[offset + i];
}

B5_tAesCtx* L1Base::GetSessionCryptoctxAesenc() {
	return &this->s[this->ptr].cryptoctx.aesenc;
}

B5_tAesCtx* L1Base::GetSessionCryptoctxAesdec() {
	return &this->s[this->ptr].cryptoctx.aesdec;
}

void L1Base::SetSessionCryptoctxHmacKey(uint8_t* keys, size_t offset, size_t len) {
	L1OutOfBoundsException boundExc;

	if (offset + len > B5_AES_256)
		throw boundExc;

	for (size_t i = offset; i < offset + len; i++)
		this->s[this->ptr].cryptoctx.hmacKey[i] = *(keys + i);
}

uint8_t* L1Base::GetSessionCryptoctxHmacKey() {
	return this->s[this->ptr].cryptoctx.hmacKey;
}

void L1Base::SetCryptoctxInizialized(bool init) {
	this->s[this->ptr].cryptoctx_initialized = init;
}

B5_tHmacSha256Ctx* L1Base::GetSessionCryptoctxHmac() {
	return &this->s[this->ptr].cryptoctx.hmac;
}

uint8_t* L1Base::GetSessionCryptoctxAuth() {
	return this->s[this->ptr].cryptoctx.auth;
}

uint8_t* L1Base::GetSessionKey() {
	return this->s[this->ptr].key;
}

void L1Base::ReadSessionBuffer(uint8_t* retData, size_t offset, size_t len) {
	L1OutOfBoundsException boundExc;

	if (offset + len > L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK)
		throw boundExc;

	memcpy(retData, this->s[this->ptr].buf + offset, len);
	//for (size_t i = 0; i < len; i++)
		//*(retData + i) = this->s[this->ptr].buf[offset + i];
}

bool L1Base::CompareSessionBuf(uint8_t* cmpData, size_t offset, size_t len) {
	L1OutOfBoundsException boundExc;

	if (offset + len > L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK)
		throw boundExc;

	if (!memcmp(cmpData, this->s[this->ptr].buf + offset, len))
		return true;

	return false;
}

void se3Algo::print(){
	std::string name((char*)this->name, L1Crypto::AlgorithmInfoSize::NAME_SIZE);
	std::cout << "\nAlgorithm: " << name << std::endl;
	std::cout << "Supported key size/sizes (bit): ";
    int i = 0;
    bool stop = false;
    do{
		if(i >= L1Crypto::AlgorithmInfoSize::KEY_NUM){ // no more than L1Crypto::AlgorithmInfoSize::KEY_NUM key sizes
		   stop = true;
		} else {
			int x = this->keySize[i]; // current key size
			if(x == 0){ // if key size is 0, we reached the end of key sizes we care about (i.e. 128, 192, 256, 0 <- end of what we need)
			   if(i == 0){
				   std::cout << "key not required for this algorithm";
			   }
			   stop = true;
			} else {
			   if(i != 0){
				   std::cout << ", ";
			   }
			   std::cout << this->keySize[i];
			}
			i++;
		}
    } while(!stop);
    std::cout << "\nBlock size: " << this->blockSize << " byte" << std::endl;
	switch(this->type){
	case L1Crypto::CryptoTypes::SE3_CRYPTO_TYPE_BLOCKCIPHER:
		std::cout << "Algorithm type: block cipher" << std::endl;
		break;
	case L1Crypto::CryptoTypes::SE3_CRYPTO_TYPE_STREAMCIPHER:
		std::cout << "Algorithm type: stream cipher" << std::endl;
		break;
	case L1Crypto::CryptoTypes::SE3_CRYPTO_TYPE_DIGEST:
		std::cout << "Algorithm type: digest" << std::endl;
		break;
	case L1Crypto::CryptoTypes::SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH:
		std::cout << "Algorithm type: block cipher with authentication and integrity" << std::endl;
		break;
	default:
		std::cout << "Algorithm type: unknown" << std::endl;
	}
}

//se3Session* L1Base::GetCurrentSession() {
//	return &(this->s[this->ptr]);
//}
