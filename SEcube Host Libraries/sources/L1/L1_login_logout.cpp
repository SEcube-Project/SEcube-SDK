/**
  ******************************************************************************
  * File Name          : L1_login_logout.cpp
  * Description        : Implementation of the LOGIN LOGOUT API.
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
 * @file	L1_login_logout.cpp
 * @date	July, 2017
 * @brief	Implementation of the LOGIN LOGOUT API
 *
 * The file contains the implementation of the LOGIN LOGOUT API
 */

#include "L1.h"
#include "L1_error_manager.h"

void L1::L1Login(const std::array<uint8_t, L1Parameters::Size::PIN>& pin, se3_access_type access, bool force) {
	uint8_t cc1[L1Parameters::Size::CHALLENGE];
	uint8_t cc2[L1Parameters::Size::CHALLENGE];
	uint16_t reqLen = 0;
	uint16_t respLen = 0;

	// copy pin to low-level array
	uint8_t pin_[L1Parameters::Size::PIN];
	memset(pin_, 0, L1Parameters::Size::PIN);
	for(int i=0; i<L1Parameters::Size::PIN; i++){
		pin_[i] = pin[i];
	}

	//prepare the data to be sent
	L0Support::Se3Rand(L1Parameters::Size::CHALLENGE, cc1);		//generate first random for challenge
	L0Support::Se3Rand(L1Parameters::Size::CHALLENGE, cc2);		//generate second random for challenge

	PrepareSessionBufferForChallenge(cc1, cc2, access);

	reqLen = L1ChallengeRequest::Offset::ACCESS + sizeof(uint16_t);
	L1LoginException loginExc;

	//send the challenge
	try {
		TXRXData(L1Commands::Codes::CHALLENGE, reqLen, 0, &respLen);
	}
	catch (L1AlreadyOpenException& e)
	{
//		printf("Debug: Device already logged in -> force? %d\n", force);
		if(force)
		{
//			printf("Debug: Try to force logout...\n");
			L1LogoutForced();
			L1Login(pin, access, false);
//			printf("Debug: Login after forced logout succeed\n");
			return;
		}
		else
		{
			throw loginExc;
		}
	}
	catch (L1Exception& e) {
		throw loginExc;
	}

	uint8_t sc[L1Parameters::Size::CHALLENGE];
	//read server challenge sc
	this->base.ReadSessionBuffer(	sc,
									L1ChallengeResponse::Offset::SC + L1Response::Offset::DATA,
									L1Parameters::Size::CHALLENGE);

	uint8_t sRespExpected[L1Parameters::Size::CHALLENGE];

	// check server response
	PBKDF2HmacSha256(pin_, L1Parameters::Size::PIN, cc1, L1Parameters::Size::CHALLENGE, L1Parameters::Parameter::ITERATIONS, sRespExpected, L1Parameters::Size::CHALLENGE);

	bool cmpRes;

	try {
		cmpRes = this->base.CompareSessionBuf(	sRespExpected,
												L1ChallengeResponse::Offset::SRESP + L1Response::Offset::DATA,
												L1Parameters::Size::CHALLENGE);
	}
	catch (L1Exception& e) {
		throw loginExc;
	}

	if (cmpRes == false)
		throw loginExc;

	//prepare key session
	//the resulting key is saved in this->base.s.key
	PBKDF2HmacSha256 (pin_,	L1Parameters::Size::PIN, cc2, L1Parameters::Size::CHALLENGE, 1, this->base.GetSessionKey(),	L1Parameters::Size::PIN);

	this->base.SetSessionLoggedIn(true);
	this->base.SetSessionAccessType((se3_access_type)access);


	//encryption can begin here
	//prepare challenge response
	PBKDF2HmacSha256 (pin_,
					  L1Parameters::Size::PIN,
					  sc,
					  L1Parameters::Size::CHALLENGE,
					  L1Parameters::Parameter::ITERATIONS,
					  this->base.GetSessionBuffer() + L1Response::Offset::DATA + L1Login::RequestOffset::CRESP,
					  L1Parameters::Size::CHALLENGE);

	reqLen = L1Parameters::Size::CHALLENGE;

	//send login command
	try {
		TXRXData(L1Commands::Codes::LOGIN, reqLen, 0, &respLen);
	}
	catch (L1Exception& e) {
		this->base.SetSessionLoggedIn(false);
		this->base.SetSessionAccessType(SE3_ACCESS_NONE);
		throw loginExc;
	}

	//read token
	this->base.SetSessionToken(L1Response::Offset::DATA + L1Login::ResponseOffset::TOKEN, L1Parameters::Size::TOKEN);
}

void L1::L1Logout() {
	L1LogoutException logOutExc;

	if (this->base.GetSessionLoggedIn() == false){
		throw logOutExc;
	}

	uint16_t dataLen = 0;
	uint16_t respLen = 0;

	try {
		TXRXData(L1Commands::Codes::LOGOUT, dataLen, 0, &respLen);
	}
	catch (L1Exception& e) {
		throw logOutExc;
	}

	this->base.SetSessionLoggedIn(false);
	this->base.SetSessionAccessType(SE3_ACCESS_NONE);
}

void L1::L1LogoutForced() {
	L1LogoutException logOutExc;

	uint16_t dataLen = 0;
	uint16_t respLen = 0;

	try {
		TXRXData(L1Commands::Codes::FORCED_LOGOUT, dataLen, 0, &respLen);
	}
	catch (L1Exception& e) {
		throw logOutExc;
	}

	this->base.SetSessionLoggedIn(false);
	this->base.SetSessionAccessType(SE3_ACCESS_NONE);
}
