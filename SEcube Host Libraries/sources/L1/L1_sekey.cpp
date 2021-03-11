/**
  ******************************************************************************
  * File Name          : L1_sekey.cpp
  * Description        : L1 APIs required to implement SEkey.
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
 * @file	L1_sekey.cpp
 * @date	July, 2017
 * @brief	L1 APIs required to implement SEkey. These must not be used explicitly.
 * @version SEcube Open Source SDK 1.5.1
 */

#include "L1.h"

using namespace std;

L1::L1(uint8_t indx) { // used by SEkey, target a specific SEcube
	this->base.InitializeSession(1);
	this->L0Open(indx);
	this->index = indx;
}

void L1::L1SEkey_Maintenance(uint8_t *buffer, uint16_t *buflen){
	if(buffer == nullptr && buflen != nullptr){
		*buflen = 0;
		return;
	}
	if(buflen == nullptr){
		throw std::invalid_argument("Cannot pass null parameters.");
	}
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op = L1Commands::Options::SE3_SEKEY_OP_GET_KEY_IDS;
	uint16_t offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	uint8_t filter = 0; // enable filter on IDs in the firmware
	this->base.FillSessionBuffer((uint8_t*)&filter, offset, 1);
	offset++;
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		*buflen = 0;
		return;
	}
	memcpy(buffer, (this->base.GetSessionBuffer()+L1Request::Offset::DATA), resp_len);
	*buflen = resp_len;
}

bool L1::L1SEkey_DeleteAllKeys(std::vector<uint32_t>& keep){
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op = L1Commands::Options::SE3_SEKEY_DELETEALL;
	uint16_t offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	for(uint32_t key : keep){ // in case some keys have to be preserved, send their IDs to the SEcube
		this->base.FillSessionBuffer((unsigned char*)&key, offset, 4);
		offset += 4;
	}
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		return false;
	}
	if(strncmp((const char*)(this->base.GetSessionBuffer()+L1Response::Offset::DATA), "OK", 2) != 0){
		return false;
	} else {
		return true;
	}
}

bool L1::L1SEkey_DeleteKey(uint32_t key_id){
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op = L1Commands::Options::SE3_SEKEY_DELETEKEY;
	uint16_t offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	this->base.FillSessionBuffer((unsigned char*)&key_id, offset, 4);
	offset += 4;
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		return false;
	}
	if(resp_len != 2 || strncmp((const char*)(this->base.GetSessionBuffer()+L1Response::Offset::DATA), "OK", 2) != 0){
		return false;
	} else {
		return true;
	}
}

bool L1::L1SEkey_isReady(){
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op = L1Commands::Options::SE3_SEKEY_ISREADY;
	uint16_t offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		return false;
	}
	if(resp_len != 2 || strncmp((const char*)(this->base.GetSessionBuffer()+L1Response::Offset::DATA), "OK", 2) != 0){
		return false;
	} else {
		return true;
	}
}

bool L1::L1SEkey_InsertKey(uint32_t key_id, uint16_t key_len, uint32_t dec_id, std::shared_ptr<uint8_t[]> key_data){
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op = L1Commands::Options::SE3_SEKEY_INSERTKEY;
	uint16_t offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	this->base.FillSessionBuffer((unsigned char*)&key_id, offset, 4);
	offset += 4;
	this->base.FillSessionBuffer((unsigned char*)&key_len, offset, 2);
	offset += 2;
	if(key_data != nullptr){ // this is in case the host wants to explicitly send the key content to the SEcube
		this->base.FillSessionBuffer((unsigned char*)&dec_id, offset, 4);
		offset+=4;
		this->base.FillSessionBuffer((unsigned char*)key_data.get(), offset, key_len);
		offset += key_len;
	}
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		return false;
	}
	if(resp_len != 2){
		return false;
	}
	char okbuf[] = "OK";
	if(memcmp((const void*)(this->base.GetSessionBuffer()+L1Response::Offset::DATA), (const void*)okbuf, 2) != 0){
		return false;
	} else {
		return true;
	}
}

bool L1::L1SEkey_Info(string& id, string& name, uint8_t mode)
{
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	unique_ptr<char[]> resp_buffer;
	uint16_t op, offset;
	if(mode != L1SEkey::Direction::STORE && mode != L1SEkey::Direction::LOAD){
		return false;
	}
	if(mode == L1SEkey::Direction::STORE){
		op = L1Commands::Options::SE3_SEKEY_OP_SETINFO;
		offset = L1Request::Offset::DATA;
		this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
		offset += 2;
		/* contenuto della richiesta: 1B per lunghezza id, id, 1B per lunghezza nome, nome */
		uint8_t idlen = id.length();
		this->base.FillSessionBuffer((unsigned char*)&idlen, offset, 1);
		offset++;
		this->base.FillSessionBuffer((unsigned char*)id.c_str(), offset, idlen);
		offset += idlen;
		uint8_t namelen = name.length();
		this->base.FillSessionBuffer((unsigned char*)&namelen, offset, 1);
		offset++;
		this->base.FillSessionBuffer((unsigned char*)name.c_str(), offset, namelen);
		offset += namelen;
		data_len = offset - L1Request::Offset::DATA;
		try{
			TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
		} catch(L1Exception& e){
			return false;
		}
		if(resp_len != 8){
			return false;
		}
		resp_buffer = make_unique<char[]>(resp_len);
		if(resp_buffer == nullptr){
			return false;
		}
		memcpy(resp_buffer.get(), this->base.GetSessionBuffer()+L1Request::Offset::DATA, resp_len);
		char tmp[] = "SEKEY_OK";
		if(memcmp(resp_buffer.get(), tmp, 8) != 0){
			resp_buffer = nullptr;
			return false;
		}
	}
	if(mode == L1SEkey::Direction::LOAD){
		op = L1Commands::Options::SE3_SEKEY_OP_GETINFO;
		offset = L1Request::Offset::DATA;
		this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
		offset += 2;
		data_len = offset - L1Request::Offset::DATA;
		try{
			TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
		} catch(L1Exception& e){
			return false;
		}
		if(resp_len == 0){
			return false;
		}
		resp_buffer.reset();
		resp_buffer = make_unique<char[]>(resp_len);
		if(resp_buffer == nullptr){
			return false;
		}
		/* contenuto della risposta: 1B per lunghezza id, id, 1B per lunghezza nome, nome */
		memcpy(resp_buffer.get(), this->base.GetSessionBuffer()+L1Request::Offset::DATA, resp_len);
		uint8_t off = 0;
		uint8_t idlen = resp_buffer[off];
		off++;
		string tmpid(&resp_buffer[off], idlen);
		off += idlen;
		uint8_t namelen = resp_buffer[off];
		off++;
		string tmpname(&resp_buffer[off], namelen);
		id = tmpid;
		name = tmpname;
	}
	return true;
}

bool L1::L1SEkey_GetKeyEnc(uint32_t key_export_id, uint32_t key_wrapping_key, std::shared_ptr<uint8_t[]>& key_export_data, uint16_t& key_export_len){
	uint16_t data_len = 0;
	uint16_t resp_len = 0;
	uint16_t op, offset;
	op = L1Commands::Options::SE3_SEKEY_OP_GETKEYENC;
	offset = L1Request::Offset::DATA;
	this->base.FillSessionBuffer((unsigned char*)&op, offset, 2);
	offset += 2;
	this->base.FillSessionBuffer((unsigned char*)&key_export_id, offset, 4);
	offset += 4;
	this->base.FillSessionBuffer((unsigned char*)&key_wrapping_key, offset, 4);
	offset += 4;
	data_len = offset - L1Request::Offset::DATA;
	try{
		TXRXData(L1Commands::Codes::SEKEY, data_len, 0, &resp_len);
	} catch(L1Exception& e){
		return false;
	}
	if(resp_len == 0){
		return false;
	}
	key_export_data.reset(); // release the memory previously used by this shared pointer (if any)
	shared_ptr<uint8_t[]> tmp(new uint8_t[resp_len]); // allocate a temporary array
	memset(tmp.get(), 0, resp_len); // clear
	memcpy((void*)tmp.get(), (const void*)(this->base.GetSessionBuffer()+L1Request::Offset::DATA), resp_len); // copy from response
	key_export_data.swap(tmp); // swap the content of tmp and key_export_data, so that the caller will have what is needed
	key_export_len = resp_len;
	return true;
}

