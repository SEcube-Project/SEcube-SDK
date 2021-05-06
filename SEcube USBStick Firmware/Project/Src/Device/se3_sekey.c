/**
  ******************************************************************************
  * File Name          : se3_sekey.c
  * Description        : Firmware functionalities to support the KMS.
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

/**
 *  \file se3_sekey.c
 *  \date 22/06/2020
 *  \version SEcube SDK 1.5.1
 *  \brief Firmware functionalities to support the KMS.
 */

#include "se3_sekey.h"
#include "se3_keys.h"
#include "se3_dispatcher_core.h" // required for login_struct

#define BUFSIZE 1024 // size of buffers used internally by load_encrypted_key_data() and insert_key()

se3_flash_it key_iterator = { .addr = NULL }; /**< Global variable required by load_key_ids() */

/*	sekey_get_implementation_info: This function would be the core of the SEkey behaviour,
 * 	it has to be implemented. The provided code is just a stub
 */
bool sekey_get_implementation_info(uint8_t* algo_implementation, uint8_t* crypto_algo, uint8_t *key){
	if(sekey_get_auth(key)){
		*algo_implementation = SE3_SECURITY_CORE;
		*crypto_algo = SE3_AES256;
	}
	return true;
}

bool sekey_get_auth(uint8_t *key){
	return true;
}

uint16_t store_user_info(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint8_t userid_len, username_len, offset = 0;
	const uint8_t *userid;
	const uint8_t *username;
    se3_flash_it it = { .addr = NULL };
    uint16_t size = 0;
    *resp_size = 0;
	if(login_struct.access != SE3_ACCESS_ADMIN){
		return SE3_ERR_ACCESS;
	}
	userid_len = req[offset];
	offset++;
	userid = (req)+offset;
	offset+=userid_len;
	username_len = req[offset];
	offset++;
	username = (req)+offset;
	if((req_size-2) != (userid_len+username_len+2)){ // +2 for id len and name len, -2 for operation choice
		return SE3_ERR_RESOURCE;
	}
	// delete all nodes in the flash of type userinfo
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)){
		if (it.type == SE3_TYPE_USERINFO){
			if (!se3_flash_it_delete(&it)) {
				return SE3_ERR_HW;
			}
		}
	}
	se3_flash_it_init(&it);
	// 1B id length, id, 1B name length, name
	size = userid_len + username_len + 2;
	if (size > SE3_FLASH_NODE_DATA_MAX) {
		return SE3_ERR_MEMORY;
	}

	if (!se3_flash_it_new(&it, SE3_TYPE_USERINFO, size)) {
		return SE3_ERR_MEMORY;
	}
	offset = 0;
	if(!se3_flash_it_write(&it, offset, &userid_len, 1)) {
		return SE3_ERR_HW;
	}
	offset++;
	if (!se3_flash_it_write(&it, offset, userid, userid_len)) {
		return SE3_ERR_HW;
	}
	offset+=userid_len;
	if(!se3_flash_it_write(&it, offset, &username_len, 1)) {
		return SE3_ERR_HW;
	}
	offset++;
	if (!se3_flash_it_write(&it, offset, username, username_len)) {
		return SE3_ERR_HW;
	}
	*resp_size = 8;
	memcpy(resp, "SEKEY_OK", 8);
	return SE3_OK;
}

uint16_t load_user_info(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint8_t userid_len, username_len, offset, counter = 0, total = 0;
	se3_flash_it it = { .addr = NULL };
	uint8_t *tmp = NULL;
	se3_flash_it_init(&it);
	*resp_size = 0;
	while(se3_flash_it_next(&it)){
		if(it.type == SE3_TYPE_USERINFO){
			if(counter != 0){
				if(tmp != NULL){
					free(tmp);
				}
				return SE3_ERR_RESOURCE;
			}
			counter++;
			offset = 0;
			memcpy(&userid_len, it.addr+offset, 1);
			offset++;
			offset+=userid_len;
			memcpy(&username_len, it.addr+offset, 1);
			total = userid_len + username_len + 2;
			if(tmp != NULL){
				free(tmp);
				tmp = NULL;
			}
			tmp = (uint8_t*)malloc(total*sizeof(uint8_t));
			if(tmp == NULL){ return SE3_ERR_MEMORY;	}
			memcpy(tmp, it.addr, total);
		}
	}
	if(counter != 1){
		if(tmp != NULL){
			free(tmp);
		}
		return SE3_ERR_RESOURCE;
	} else {
		if(tmp != NULL){
			memcpy(resp, tmp, total);
			*resp_size = total;
			free(tmp);
			return SE3_OK;
		} else {
			return SE3_ERR_RESOURCE;
		}
	}
}

uint16_t load_encrypted_key_data(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t wrapped_key_id = 0, curr_key_id = 0, wrapping_key_id = 0;
	uint16_t key_len = 0; // length of wrapped key
	uint8_t *wrapped_key_data = NULL;
	int counter = 0;
	se3_flash_it it = { .addr = NULL };
	*resp_size = 0;

	// preliminary check
	if(login_struct.access != SE3_ACCESS_ADMIN){
		return SE3_ERR_ACCESS; // admin privilege level required
	}
	if((req_size - 2) != 8){
		return SE3_ERR_PARAMS; // check if the request has the right size (do not consider OP code)
	}

	// get key IDs
	memcpy(&wrapped_key_id, req, 4); // ID of key to be wrapped
	memcpy(&wrapping_key_id, req+4, 4); // ID of key wrapping key
	if(wrapping_key_id == ZERO_ID || wrapping_key_id == NULL_ID || wrapped_key_id == ZERO_ID || wrapped_key_id == NULL_ID){
		return SE3_ERR_PARAMS;
	}

	// scan flash searching for wrapped key
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)) {
		/* this implementation scans the entire flash memory searching for the key, therefore it is sub-optimal.
		 * in order to enhance the performance this can be modified returning as soon as the key is found; but
		 * this would not detect any duplicated key ID. */
		if (it.type == SE3_TYPE_KEY) {
			SE3_GET32(it.addr, SE3_FLASH_KEY_OFF_ID, curr_key_id);
			if (curr_key_id == wrapped_key_id) {
				if(counter != 0){ // found duplicated key ID...error
					if(wrapped_key_data != NULL){
						free(wrapped_key_data);
					}
					return SE3_ERR_RESOURCE;
				}
				counter++; // counter of keys with the ID we are looking for
				SE3_GET16(it.addr, SE3_FLASH_KEY_OFF_DATA_LEN, key_len);
				if(wrapped_key_data != NULL){ // before malloc, free keycontent if, for any reason, it is not NULL
					free(wrapped_key_data);
					wrapped_key_data = NULL;
				}
				wrapped_key_data = (uint8_t*)malloc(key_len);
				if(wrapped_key_data == NULL){
					return SE3_ERR_MEMORY;
				}
				memcpy(wrapped_key_data, it.addr+SE3_FLASH_KEY_OFF_DATA, key_len);
			}
		}
	}
	if(counter != 1){ // check if key (only 1) was found
		if(wrapped_key_data != NULL){
			free(wrapped_key_data);
		}
		return SE3_ERR_RESOURCE;
	}

	/* encrypt the value of keycontent with the wrapping key */
	uint8_t request[BUFSIZE], response[BUFSIZE];
	memset(request, 0, BUFSIZE); // request buffer for crypto_init() and crypto_update()
	memset(response, 0, BUFSIZE); // response buffer for crypto_init() and crypto_update()
	uint16_t response_size = 0; // response for crypto_init() and crypto_update()
	uint16_t algo = SE3_ALGO_AES; // key is wrapped using AES-CBC
	uint16_t mode = SE3_FEEDBACK_CBC | SE3_DIR_ENCRYPT;
	uint32_t sessionId = SE3_SESSION_INVALID; // session returned by crypto_init()
	// write crypto_init() parameters to request buffer
	memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_ALGO, &algo, 2);
	memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_MODE, &mode, 2);
	memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_KEY_ID, &wrapping_key_id, 4);
	// initialise crypto context
	uint16_t rc = crypto_init(SE3_CMD1_CRYPTO_INIT_REQ_SIZE, request, &response_size, response);
	if((rc != SE3_OK) || (response_size != SE3_CMD1_CRYPTO_INIT_RESP_SIZE)){
		if(wrapped_key_data != NULL){
			free(wrapped_key_data);
		}
		return SE3_ERR_RESOURCE;
	}

	memcpy(&sessionId, response+SE3_CMD1_CRYPTO_INIT_RESP_OFF_SID, response_size); // get session id from crypto_init()
	uint8_t padding = (B5_AES_BLK_SIZE - (key_len % B5_AES_BLK_SIZE)); // compute how much padding we need for encryption according to PKCS#7
	size_t plaintext_size = key_len + padding; // total size of buffer to be encrypted, including padding

	// allocate buffer to store wrapped key and padding
	uint8_t *plaintext_data = NULL;
	plaintext_data = malloc(plaintext_size);
	if(plaintext_data == NULL){
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		return SE3_ERR_RESOURCE;
	} else {
		memset(plaintext_data, 0, plaintext_size);
		memcpy(plaintext_data, wrapped_key_data, key_len); // copy value of wrapped key
		memset(plaintext_data+key_len, padding, padding); // set padding according to PKCS#7
	}

	// allocate buffer to store encrypted value of wrapped key
	uint8_t *ciphertext_data = NULL;
	size_t ciphertext_size = plaintext_size + B5_AES_BLK_SIZE; // consider ciphertext + initialisation vector
	ciphertext_data = malloc(ciphertext_size);
	if(ciphertext_data == NULL){
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		if(plaintext_data != NULL){ free(plaintext_data); }
		return SE3_ERR_RESOURCE;
	} else {
		memset(ciphertext_data, 0, ciphertext_size);
	}

	// generate random initialisation vector for AES-CBC
	uint8_t iv[B5_AES_BLK_SIZE];
	memset(iv, 0, B5_AES_BLK_SIZE);
	if(se3_rand(B5_AES_BLK_SIZE, iv) != B5_AES_BLK_SIZE){
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		if(plaintext_data != NULL){ free(plaintext_data); }
		if(ciphertext_data != NULL){ free(ciphertext_data); }
		return SE3_ERR_RESOURCE;
	}

	// setup initialisation vector
	uint16_t flags = SE3_CRYPTO_FLAG_SETIV;
	uint16_t iv_len = B5_AES_BLK_SIZE;
	memset(request, 0, BUFSIZE);
	memset(response, 0, BUFSIZE);
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID, &sessionId, 4); // first 4 byte for session id
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS, &flags, 2); // 2 bytes for flags
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN1_LEN, &iv_len, 2); // 2 bytes for datain1_len
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA, iv, iv_len); // copy initialisation vector
	uint16_t this_req_size = 0;
	this_req_size = 4 + 2 + 2 + iv_len;
	/* request is only partially filled because we set only the bytes we need, everything else has already been set to 0,
	   therefore it will be ignored by crypto_update (). */
	rc = crypto_update(this_req_size, request, &response_size, response);
	if(rc != SE3_OK){
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		if(plaintext_data != NULL){ free(plaintext_data); }
		if(ciphertext_data != NULL){ free(ciphertext_data); }
		return SE3_ERR_RESOURCE;
	}

	// copy initialisation vector to first part of our encrypted data (iv must be visible in clear)
	memcpy(ciphertext_data, iv, B5_AES_BLK_SIZE);

	// do actual AES-CBC encryption
	flags = SE3_CRYPTO_FLAG_FINIT;
	memset(request, 0, BUFSIZE);
	memset(response, 0, BUFSIZE);
	/* request is only partially filled because we set only the bytes we need, everything else has already been set to 0,
	   therefore it will be ignored by crypto_update (). */
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID, &sessionId, 4);
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS, &flags, 2);
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN2_LEN, &plaintext_size, 2);
	memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA, plaintext_data, plaintext_size);
	this_req_size = 4 + 2 + 2 + plaintext_size;
	rc = crypto_update(this_req_size, request, &response_size, response);
	if(rc != SE3_OK){
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		if(plaintext_data != NULL){ free(plaintext_data); }
		if(ciphertext_data != NULL){ free(ciphertext_data); }
		return SE3_ERR_RESOURCE;
	}

	// parse response buffer after crypto_update()
	uint16_t encrypted_size = 0;
	memcpy(&encrypted_size, response+SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATAOUT_LEN, 2);
	if(encrypted_size != (ciphertext_size - B5_AES_BLK_SIZE)){ // do not consider length of iv in ciphertext_size
		if(wrapped_key_data != NULL){ free(wrapped_key_data); }
		if(plaintext_data != NULL){ free(plaintext_data); }
		if(ciphertext_data != NULL){ free(ciphertext_data); }
		return SE3_ERR_RESOURCE;
	}
	memcpy(ciphertext_data+B5_AES_BLK_SIZE, response+SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA, encrypted_size);

	// write data to resp buffer
	memcpy(resp, ciphertext_data, ciphertext_size);
	*resp_size = ciphertext_size;

	// free memory and return
	if(wrapped_key_data != NULL){ free(wrapped_key_data); }
	if(plaintext_data != NULL){ free(plaintext_data); }
	if(ciphertext_data != NULL){ free(ciphertext_data); }
	return SE3_OK;
}

uint16_t load_key_ids(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t key_id = 0;
	uint16_t offset = 0, keylen = 0;
	*resp_size = 0;
	bool filter = false; // activate filter to return only the key IDs that can be managed manually
	if(req_size != 3){
		return SE3_ERR_PARAMS;
	}
	if(req[2] == 1){
		filter = true;
	}
	do {
		/* 7536 is the limit of data we can write in requests and responses to/from the SEcube */
		if(*resp_size >= 7532){ // max is 7536 but we must keep 4 bytes available for ID=0 sent in case we reach the end of the flash (see below at the end of the function)
			return SE3_OK; // still in the middle of the flash...not all IDs returned (but return here because we can't go past 7536 bytes, next time we will start from next flash sector)
		}
		if (key_iterator.addr != NULL && key_iterator.type == SE3_TYPE_KEY) {
			SE3_GET32(key_iterator.addr, SE3_FLASH_KEY_OFF_ID, key_id); // get key ID and copy it to the response buffer
			SE3_GET16(key_iterator.addr, SE3_FLASH_KEY_OFF_DATA_LEN, keylen); // get key length
			if((filter == false) || ((filter == true) && key_id >= MANUAL_ID_BEGIN && key_id <= MANUAL_ID_END)){
				memcpy(resp + offset, &key_id, 4);
				offset += 4;
				memcpy(resp + offset, &keylen, 2);
				offset += 2;
				*resp_size = (*resp_size) + 6;
			}
		}
	} while (se3_flash_it_next(&key_iterator));
	/* reset the iterator to the beginning of the flash (required for next call of load_key_ids).
	 * notice that this is done only if we reach the end of the flash (we scanned all IDs)...if we still
	 * haven't scanned all IDs, we won't execute the next line because we will return with the instruction
	 * few lines above. */
	se3_flash_it_init(&key_iterator);
	memset(resp + offset, 0, 6); // put all zeroes as the last id (id = 0 is not valid so the host side will understand that we reached the end of the flash)
	*resp_size = (*resp_size) + 6;
	return SE3_OK;
}

uint16_t delete_all_keys(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t key_id = 0;
	bool error_ = false, skip = false;
	se3_flash_it it = { .addr = NULL };
	uint32_t tokeep = (req_size-2) / 4; // condition: each key ID is stored on 4 bytes
	uint32_t *keep = NULL;
	uint16_t offset = 0;
	*resp_size = 0;
	if(((req_size-2) % 4) != 0){
		return SE3_ERR_PARAMS;
	}
	keep = (uint32_t*)malloc(tokeep * sizeof(uint32_t));
	if(keep == NULL){
		return SE3_ERR_MEMORY;
	}
	for(int i=0; i<tokeep; i++){
		memcpy(&keep[i], req+offset, 4);
		offset+=4;
	}
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)){
		if (it.type == SE3_TYPE_KEY){
			SE3_GET32(it.addr, SE3_FLASH_KEY_OFF_ID, key_id);
			/* delete all keys except keys specified by the host */
			for(int i=0; i<tokeep; i++){
				if(keep[i] == key_id){
					skip = true;
					break;
				}
			}
			if(skip){
				skip = false;
				continue;
			}
			if (!se3_flash_it_delete(&it)) {
				error_ = true;
			}
		}
	}
	if(keep != NULL){
		free(keep);
	}
	if(error_){
		return SE3_ERR_RESOURCE;
	} else {
		memcpy(resp, "OK", 2);
		*resp_size = 2;
		return SE3_OK;
	}
}

uint16_t delete_key(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t key_id = 0, kid = 0;
	bool error_ = false;
	se3_flash_it it = { .addr = NULL };
	*resp_size = 0;
	if((req_size - 2) != 4){
		return SE3_ERR_PARAMS;
	}
	memcpy(&kid, req, 4); // retrieve the key id from the input buffer
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)){
		if (it.type == SE3_TYPE_KEY){
			SE3_GET32(it.addr, SE3_FLASH_KEY_OFF_ID, key_id);
			if(key_id == kid){
				if (!se3_flash_it_delete(&it)) {
					error_ = true;
				}
			}
		}
	}
	if(error_){
		return SE3_ERR_RESOURCE;
	} else {
		memcpy(resp, "OK", 2);
		*resp_size = 2;
		return SE3_OK;
	}
	/* notice that if the key specified by the host is not found in the SEcube flash we simply return ok because
	 * in the end the goal is already reached, in fact we don't even have that key. */
}

uint16_t is_ready(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t k = 0;
	*resp_size = 0;
	bool dbkey = false;
	bool wckey = false;
	se3_flash_it it = { .addr = NULL };
	se3_flash_it_init(&it);
	while (se3_flash_it_next(&it)){
		if (it.type == SE3_TYPE_KEY){
			SE3_GET32(it.addr, SE3_FLASH_KEY_OFF_ID, k);
			if(k == RESERVED_ID_SEKEY_SECUREDB){
				dbkey = true; // there is the key for the encrypted metadata db
			}
			if(k == RESERVED_ID_SEKEY_WILDCARD){
				wckey = true; // there is the wildcard key for the updates
			}
		}
	}
	if(dbkey && wckey){
		memcpy(resp, "OK", 2);
		*resp_size = 2;
	}
	return SE3_OK;
}

uint16_t insert_key(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp){
	uint32_t key_id = 0; // id of the key to be stored on the device
	uint32_t wrapping_key_id = 0; // id of key used to encrypt the key provided by the caller (if any)
	uint16_t key_data_len = 0; // length of key provided by the caller (this may be encrypted)
	uint8_t *key_data = NULL;
	const uint8_t *keyptr;
    se3_flash_key key;
    bool equal = false;
	se3_flash_it it = { .addr = NULL };
	*resp_size = 0;

	// preliminary check
	if((req_size-2) < 6){ // minimum size is 4B for ID and 2B for key length
		return SE3_ERR_PARAMS;
	}

	// parse request
	memcpy(&key_id, req, 4); // key id
	memcpy(&key_data_len, req+4, 2); // key length
	key_data = (uint8_t*)malloc(key_data_len); // allocate space for the key content
	if(key_data == NULL){
		return SE3_ERR_MEMORY;
	} else {
		memset(key_data, 0, key_data_len);
	}

	// depending on request size, store the provided key in the SEcube
	if((req_size-2) == 6){ // only ID and size provided, key generation happens internally with TRNG

		/* The internal computation of the key is performed with the TRNG of the SEcube. This is sufficient to ensure a high enough entropy to the key value.
		 * For more informations about key generation details look at the NIST SP 800-13 Rev.1 (https://csrc.nist.gov/publications/detail/sp/800-133/rev-1/final),
		 * in particular Section 4 (option 1 from section 4 is used here). */

		if(se3_rand(key_data_len, key_data) != key_data_len){
			if(key_data != NULL){ free(key_data);	}
			return SE3_ERR_HW;
		}		
	} else {

		// the caller provided ID, key size and key data (+ id of wrapping key if key data is encrypted. wrapping key = 0 if key data in clear, != 0 if encrypted).

		if((req_size-2) != (4+2+4+key_data_len)){ // 4B for key ID, 2B for key data len, 4 bytes for ID of wrapping key
			if(key_data != NULL){ free(key_data);	}
			return SE3_ERR_PARAMS;
		}
		memcpy(&wrapping_key_id, req+6, 4); // get id of wrapping key from request buffer
		keyptr = req + 10; // memory address of key data passed in input

		if(wrapping_key_id == 0){ // key was sent as plaintext
			memcpy(key_data, keyptr, key_data_len);
		} else {

			/* key was sent in encrypted form. same as load_encrypted_key_data() but opposite direction */

			uint8_t request[BUFSIZE], response[BUFSIZE];
			memset(request, 0, BUFSIZE);
			memset(response, 0, BUFSIZE);
			uint16_t response_size = 0; // size of response by crypto_init() and crypto_update()
			uint16_t algo = SE3_ALGO_AES; // decrypt using AES-CBC
			uint16_t mode = SE3_FEEDBACK_CBC | SE3_DIR_DECRYPT;
			//uint16_t d1len = 0, d2len = newLen, dec_len = 0;
			uint32_t sessionId = SE3_SESSION_INVALID; // crypto session set by crypto_init()

			// initialise crypto context
			memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_ALGO, &algo, 2);
			memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_MODE, &mode, 2);
			memcpy(request + SE3_CMD1_CRYPTO_INIT_REQ_OFF_KEY_ID, &wrapping_key_id, 4);
			uint16_t rc = crypto_init(SE3_CMD1_CRYPTO_INIT_REQ_SIZE, request, &response_size, response);
			if((rc != SE3_OK) || (response_size != SE3_CMD1_CRYPTO_INIT_RESP_SIZE)){
				if(key_data != NULL){ free(key_data); }
				return SE3_ERR_RESOURCE;
			}
			memcpy(&sessionId, response+SE3_CMD1_CRYPTO_INIT_RESP_OFF_SID, response_size); // get session id from crypto_init()

			// get initialisation vector from input data, set iv for AES-CBC
			uint8_t iv[B5_AES_BLK_SIZE];
			memset(iv, 0, B5_AES_BLK_SIZE);
			memcpy(iv, keyptr, B5_AES_BLK_SIZE); // input data: IV (16B) + encrypted key
			uint16_t flags = SE3_CRYPTO_FLAG_SETIV;
			uint16_t iv_len = B5_AES_BLK_SIZE;
			memset(request, 0, BUFSIZE);
			memset(response, 0, BUFSIZE);
			uint16_t this_req_size = 0;
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID, &sessionId, 4); // first 4 byte for session id
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS, &flags, 2); // 2 bytes for flags
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN1_LEN, &iv_len, 2); // 2 bytes for datain1_len
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA, iv, iv_len); // copy initialisation vector
			this_req_size = 4 + 2 + 2 + iv_len;
			rc = crypto_update(this_req_size, request, &response_size, response);
			if(rc != SE3_OK){
				if(key_data != NULL){ free(key_data); }
				return SE3_ERR_RESOURCE;
			}

			// do actual AES-CBC decryption
			uint16_t wrapped_len = key_data_len - B5_AES_BLK_SIZE; // do not consider iv
			flags = SE3_CRYPTO_FLAG_FINIT;
			memset(request, 0, BUFSIZE);
			memset(response, 0, BUFSIZE);
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID, &sessionId, 4);
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS, &flags, 2);
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN2_LEN, &wrapped_len, 2);
			memcpy(request+SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA, keyptr+B5_AES_BLK_SIZE, wrapped_len); // go past parameters iv
			this_req_size = 4 + 2 + 2 + wrapped_len;
			rc = crypto_update(this_req_size, request, &response_size, response);
			if(rc != SE3_OK){
				if(key_data != NULL){ free(key_data); }
				return SE3_ERR_RESOURCE;
			}

			// parse response buffer after crypto_update()
			uint16_t decrypted_size = 0;
			memcpy(&decrypted_size, response+SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATAOUT_LEN, 2);
			if(decrypted_size != wrapped_len){
				if(key_data != NULL){ free(key_data); }
				return SE3_ERR_RESOURCE;
			}
			uint8_t padding = response[SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA+decrypted_size-1]; // read how much padding (PKCS#7, last byte)

			// save decrypted key content into key_data
			memcpy(key_data, response+SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA, decrypted_size-padding);
			key_data_len = decrypted_size-padding; // update real length of key
		}
	}

	// now everything is ready to store the key in the SEcube
	key.id = key_id;
	key.data_size = key_data_len;
	key.data = key_data;

	/* strategy for key insertion into flash: retrieve the data sent by the host, check if in memory there is already
	 * a key with the same id and the same content, if the key is already there (same id, same length, same key value...)
	 * do nothing and return ok. if the key is already there (same id) but is not exactly equal or if the key is not there
	 * (key with that id not found in flash) then delete the flash block (if key id is equal but key content is not the same)
	 * and create the new key, finally return ok. */

	se3_flash_it_init(&it);
	if (!se3_key_find(key.id, &it)) { // search in the flash memory if a key with the same ID is already present
		it.addr = NULL;
	}
	if (NULL != it.addr) { // enter if there's another key with same ID
		equal = se3_key_equal(&it, &key);  // do not replace if equal
		if (!equal) { // if not equal delete current key
			if (!se3_flash_it_delete(&it)) {
				if(key_data != NULL){
					free(key_data);
				}
				return SE3_ERR_HW;
			}
		}
	}
	it.addr = NULL;
	if (!equal) { // if not equal create new key
		if (!se3_key_new(&it, &key)) {
			if(key_data != NULL){
				free(key_data);
			}
			return SE3_ERR_MEMORY;
		}
	}
	if(key_data != NULL){
		free(key_data);
	}
	*resp_size = 2;
	memcpy(resp, "OK", 2);
	return SE3_OK;
}
