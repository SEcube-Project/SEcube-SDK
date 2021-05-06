/**
  ******************************************************************************
  * File Name          : se3_dispatcher_core.h
  * Description        : Firmware command dispatcher
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

#include "se3_dispatcher_core.h"
#include "se3_communication_core.h"

uint8_t algo_implementation;
uint8_t crypto_algo;

SE3_LOGIN_STATUS login_struct;

static void login_cleanup();
bool key_len_valid(uint16_t len);

/* simple dispatcher for sekey-related operations */
uint16_t sekey_utilities(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    uint16_t operation; // the type of operation to be executed
    memcpy((void*)&(operation), (void*)req, 2);
    se3_flash_it it = { .addr = NULL};
    if(!login_struct.y){
        return SE3_ERR_ACCESS;
    }
    se3_flash_it_init(&it);
    it.addr = NULL;
    switch (operation) {
        case SE3_SEKEY_OP_SETINFO:
            return store_user_info(req_size, req+2, resp_size, resp);
            break;
        case SE3_SEKEY_OP_GETINFO:
            return load_user_info(req_size, req+2, resp_size, resp);
            break;
        case SE3_SEKEY_OP_GETKEYENC:
			return load_encrypted_key_data(req_size, req+2, resp_size, resp);
			break;
        case SE3_SEKEY_OP_GET_KEY_IDS:
        	return load_key_ids(req_size, req, resp_size, resp);
        	break;
        case SE3_SEKEY_DELETEALL:
        	return delete_all_keys(req_size, req+2, resp_size, resp);
        	break;
        case SE3_SEKEY_DELETEKEY:
        	return delete_key(req_size, req+2, resp_size, resp);
        	break;
        case SE3_SEKEY_INSERTKEY:
        	return insert_key(req_size, req+2, resp_size, resp);
        	break;
        case SE3_SEKEY_ISREADY:
        	return is_ready(req_size, req+2, resp_size, resp);
        	break;
        default:
            SE3_TRACE(("[sekey_utilities] invalid operation\n"));
            return SE3_ERR_PARAMS;
    }
    return SE3_OK;
}

uint16_t error(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    return SE3_ERR_CMD;
}

/** \brief set or get configuration record
 *
 *  config : (type:ui16, op:ui16, value[32]) => (value[32])
 */
uint16_t config(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    struct {
        uint16_t type;
        uint16_t op;
        const uint8_t* value;
    } req_params;
    struct {
        uint8_t* value;
    } resp_params;

    SE3_TRACE(("[se3_dispatcher_core.c] config"));

    if (!login_struct.y) {
        SE3_TRACE(("[config] not logged in\n"));
        return SE3_ERR_ACCESS;
    }

    SE3_GET16(req, SE3_CMD1_CONFIG_REQ_OFF_ID, req_params.type);
    SE3_GET16(req, SE3_CMD1_CONFIG_REQ_OFF_OP, req_params.op);
    req_params.value = req + SE3_CMD1_CONFIG_REQ_OFF_VALUE;
    resp_params.value = resp + SE3_CMD1_CONFIG_RESP_OFF_VALUE;

    // check params
    if (req_params.type >= SE3_RECORD_MAX) {
        SE3_TRACE(("[config] type out of range\n"));
        return SE3_ERR_PARAMS;
    }
    switch (req_params.op) {
    case SE3_CONFIG_OP_GET:
    case SE3_CONFIG_OP_SET:
        if (req_size != SE3_CMD1_CONFIG_REQ_OFF_VALUE + SE3_RECORD_SIZE) {
            SE3_TRACE(("[config] req size mismatch\n"));
            return SE3_ERR_PARAMS;
        }
        break;
    default:
        SE3_TRACE(("[config] op invalid\n"));
        return SE3_ERR_PARAMS;
    }

    if (req_params.op == SE3_CONFIG_OP_GET) {
        // check access
        if (login_struct.access < se3_security_info.records[req_params.type].read_access) {
            SE3_TRACE(("[config] insufficient access\n"));
            return SE3_ERR_ACCESS;
        }
        if (!record_get(req_params.type, resp_params.value)) {
            return SE3_ERR_RESOURCE;
        }
        *resp_size = SE3_RECORD_SIZE;
    }
    else if (req_params.op == SE3_CONFIG_OP_SET) {
        // check access
        if (login_struct.access < se3_security_info.records[req_params.type].write_access) {
            SE3_TRACE(("[config] insufficient access\n"));
            return SE3_ERR_ACCESS;
        }
        if (!record_set(req_params.type, req_params.value)) {
            return SE3_ERR_MEMORY;
        }
    }
    else {
        SE3_TRACE(("[config] invalid op\n"));
        return SE3_ERR_PARAMS;
    }

	return SE3_OK;
}

/*
    Challenge-based authentication

	Password-Based Key Derivation Function 2
			PBKDF2(PRF, Password, Salt, c, dkLen)

	cc1     client(=host) challenge 1
			random(32)
	cc2     client(=host) challenge 2
			random(32)
	sc      server(=device) challenge
			random(32)
	cresp   client(=host) response
			PBKDF2(HMAC-SHA256, pin, sc, SE3_CHALLENGE_ITERATIONS, SE3_CHALLENGE_SIZE)
	sresp   server(=device) response
			PBKDF2(HMAC-SHA256, pin, cc1, SE3_CHALLENGE_ITERATIONS, SE3_CHALLENGE_SIZE)
	key     session key for enc/auth of protocol
			PBKDF2(HMAC-SHA256, pin, cc2, 1, SE3_PIN_SIZE)

	challenge (not encrypted)
		host
			generate cc1,cc2
			send cc1,cc2
		device
			generate sc
			compute sresp, cresp, key
			send sresp
	login (encrypted with key)
		host
			compute sresp, cresp, key
			check sresp
			send cresp
		device
			check cresp
			send token  <- the token is transmitted encrypted
*/

/** \brief Get a login challenge from the server
 *
 *  challenge : (cc1[32], cc2[32], access:ui16) => (sc[32], sresp[32])
 */
uint16_t challenge(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    uint8_t pin[SE3_PIN_SIZE];
    struct {
        const uint8_t* cc1;
        const uint16_t access;
        const uint8_t* cc2;
    } req_params;
    struct {
        uint8_t* sc;
        uint8_t* sresp;
    } resp_params;

    if (req_size != SE3_CMD1_CHALLENGE_REQ_SIZE) {
        SE3_TRACE(("[challenge] req size mismatch\n"));
        return SE3_ERR_PARAMS;
    }

    req_params.cc1 = req + SE3_CMD1_CHALLENGE_REQ_OFF_CC1;
    req_params.cc2 = req + SE3_CMD1_CHALLENGE_REQ_OFF_CC2;
    SE3_GET16(req, SE3_CMD1_CHALLENGE_REQ_OFF_ACCESS, req_params.access);
    resp_params.sc = resp + SE3_CMD1_CHALLENGE_RESP_OFF_SC;
    resp_params.sresp = resp + SE3_CMD1_CHALLENGE_RESP_OFF_SRESP;

	if (login_struct.y) {
		SE3_TRACE(("[challenge] already logged in"));
		return SE3_ERR_STATE;
	}

    // default pin is zero, if no record is found
    memset(pin, 0, SE3_PIN_SIZE);
    switch (req_params.access) {
    case SE3_ACCESS_USER:
        record_get(SE3_RECORD_TYPE_USERPIN, pin);
        break;
    case SE3_ACCESS_ADMIN:
        record_get(SE3_RECORD_TYPE_ADMINPIN, pin);
        break;
    default:
    	SE3_TRACE(("[se3_dispatcher_core.c - challenge] default error login"));
        return SE3_ERR_PARAMS;
	}

	if (SE3_CHALLENGE_SIZE != se3_rand(SE3_CHALLENGE_SIZE, resp_params.sc)) {
		SE3_TRACE(("[challenge] se3_rand failed"));
		return SE3_ERR_HW;
	}

	// cresp = PBKDF2(HMACSHA256, pin, sc, SE3_CHALLENGE_ITERATIONS, SE3_CHALLENGE_SIZE)
	PBKDF2HmacSha256(pin, SE3_PIN_SIZE, resp_params.sc,	SE3_CHALLENGE_SIZE, SE3_CHALLENGE_ITERATIONS, login_struct.challenge, SE3_CHALLENGE_SIZE);

	// sresp = PBKDF2(HMACSHA256, pin, cc1, SE3_CHALLENGE_ITERATIONS, SE3_CHALLENGE_SIZE)
	PBKDF2HmacSha256(pin, SE3_PIN_SIZE, req_params.cc1,	SE3_CHALLENGE_SIZE, SE3_CHALLENGE_ITERATIONS, resp_params.sresp, SE3_CHALLENGE_SIZE);

	// key = PBKDF2(HMACSHA256, pin, cc2, 1, SE3_PIN_SIZE)
	PBKDF2HmacSha256(pin, SE3_PIN_SIZE, req_params.cc2, SE3_CHALLENGE_SIZE, 1, login_struct.key, SE3_PIN_SIZE);

	login_struct.challenge_access = req_params.access;

    *resp_size = SE3_CMD1_CHALLENGE_RESP_SIZE;
	return SE3_OK;
}

/** \brief respond to challenge, completing login
 *
 *  login : (cresp[32]) => (tok[16])
 */
uint16_t login(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    struct {
        const uint8_t* cresp;
    } req_params;
    struct {
        uint8_t* token;
    } resp_params;
    uint16_t access;

    if (req_size != SE3_CMD1_LOGIN_REQ_SIZE) {
        SE3_TRACE(("[login] req size mismatch\n"));
        return SE3_ERR_PARAMS;
    }

	if (login_struct.y) {
		SE3_TRACE(("[login] already logged in"));
		return SE3_ERR_STATE;
	}
	if (SE3_ACCESS_MAX == login_struct.challenge_access) {
		SE3_TRACE(("[login] not waiting for challenge response"));
		return SE3_ERR_STATE;
	}

    req_params.cresp = req + SE3_CMD1_LOGIN_REQ_OFF_CRESP;
    resp_params.token = resp + SE3_CMD1_LOGIN_RESP_OFF_TOKEN;

	access = login_struct.challenge_access;
	login_struct.challenge_access = SE3_ACCESS_MAX;
	if (memcmp(req_params.cresp, (uint8_t*)login_struct.challenge, 32)) {
		SE3_TRACE(("[login] challenge response mismatch"));
		return SE3_ERR_PIN;
	}

	if (SE3_TOKEN_SIZE != se3_rand(SE3_TOKEN_SIZE, (uint8_t*)login_struct.token)) {
		SE3_TRACE(("[login] random failed"));
		return SE3_ERR_HW;
	}
	memcpy(resp_params.token, (uint8_t*)login_struct.token, 16);
	login_struct.y = 1;
	login_struct.access = access;

    *resp_size = SE3_CMD1_LOGIN_RESP_SIZE;
	return SE3_OK;
}

/** \brief Log out and release resources
 *
 *  logout : () => ()
 */
uint16_t logout(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    if (req_size != 0) {
        SE3_TRACE(("[logout] req size mismatch\n"));
        return SE3_ERR_PARAMS;
    }
	if (!login_struct.y) {
		SE3_TRACE(("[logout] not logged in\n"));
		return SE3_ERR_ACCESS;
	}
	login_cleanup();
	return SE3_OK;
}

bool key_len_valid(uint16_t len){
	uint16_t bitlen = len*8;
	switch(bitlen){
		case 128:
			return true;
		case 192:
			return true;
		case 256:
			return true;
		default:
			return false;
	}
}

uint16_t key_edit(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    struct {
        uint16_t op;
        uint32_t id;
        uint16_t data_len;
        const uint8_t* data;
    } req_params;
    se3_flash_key key;
    se3_flash_it it = { .addr = NULL };
    uint8_t *trng_keydata = NULL;

    if (req_size < SE3_CMD1_KEY_EDIT_REQ_OFF_DATA) {
        SE3_TRACE(("[key_edit] req size mismatch\n"));
        return SE3_ERR_PARAMS;
    }
    if (!login_struct.y) {
        SE3_TRACE(("[key_edit] not logged in\n"));
        return SE3_ERR_ACCESS;
    }

    SE3_GET16(req, SE3_CMD1_KEY_EDIT_REQ_OFF_OP, req_params.op); // get type of operation
    SE3_GET32(req, SE3_CMD1_KEY_EDIT_REQ_OFF_ID, req_params.id); // get key ID
    SE3_GET16(req, SE3_CMD1_KEY_EDIT_REQ_OFF_DATA_LEN, req_params.data_len); // get key length

    // check the validity of key length depending on combination with op
    if((req_params.op == SE3_KEY_OP_DELETE) && (req_params.data_len != 0)){
    	SE3_TRACE(("[se3_dispatcher_core.c - key_edit] error parameters"));
    	return SE3_ERR_PARAMS;
    }
    if((req_params.op != SE3_KEY_OP_DELETE) && !key_len_valid(req_params.data_len)){
    	SE3_TRACE(("[se3_dispatcher_core.c - key_edit] error parameters"));
    	return SE3_ERR_PARAMS;
    }
    if(req_params.op != SE3_KEY_OP_ADD){ // get key value only if required
    	req_params.data = NULL;
    } else {
    	req_params.data = req + SE3_CMD1_KEY_EDIT_REQ_OFF_DATA;
    }

    // copy values in key structure
    key.id = req_params.id;
    key.data_size = req_params.data_len;
    key.data = (uint8_t*)req_params.data;

    /* check if key ID meets requirements
     * this function is dedicated to manual key management, therefore we do not allow
     * any key ID that should be handled by SEkey or that is not valid. */
    if((key.id == NULL_ID) ||
       (key.id == ZERO_ID) ||
	   (key.id >= SEKEY_ID_BEGIN && key.id <= SEKEY_ID_END) ||
	   (key.id >= RESERVED_ID_SEKEY_BEGIN && key.id <= RESERVED_ID_SEKEY_END)){
    	return SE3_ERR_PARAMS;
    }

    // check if there is already a key with same ID
    se3_flash_it_init(&it);
    if (!se3_key_find(key.id, &it)) {
        it.addr = NULL;
    }

    // execute operation requested by the host
    switch (req_params.op) {
		case SE3_KEY_OP_ADD:
			if (NULL != it.addr) { // key already exists
				return SE3_ERR_RESOURCE;
			}
			if (!se3_key_new(&it, &key)) {
				SE3_TRACE(("[key_edit] se3_key_new failed\n"));
				return SE3_ERR_MEMORY;
			}
			break;
		case SE3_KEY_OP_DELETE: // if keys does not exist do not return any error
			if (it.addr != NULL) {
				if (!se3_flash_it_delete(&it)) {
					return SE3_ERR_HW;
				}
			}
			break;
		case SE3_KEY_OP_ADD_TRNG:
			if (NULL != it.addr) { // key already exists
				return SE3_ERR_RESOURCE;
			}
			trng_keydata = (uint8_t*)malloc(key.data_size); // allocate space for key content generated with TRNG
			if(trng_keydata == NULL){ return SE3_ERR_MEMORY; }
			memset(trng_keydata, 0, key.data_size);
			if(!se3_rand(key.data_size, trng_keydata)){ // generate random data with TRNG
				SE3_TRACE(("[key_edit] se3_rand failed\n"));
				if(trng_keydata != NULL){ free(trng_keydata); }
				return SE3_ERR_MEMORY;
			}
			key.data = trng_keydata;
			if (!se3_key_new(&it, &key)) { // store key
				SE3_TRACE(("[key_edit] se3_key_new failed\n"));
				if(trng_keydata != NULL){ free(trng_keydata); }
				return SE3_ERR_MEMORY;
			}
			if(trng_keydata != NULL){ free(trng_keydata); }
			break;
		default:
			SE3_TRACE(("[key_edit] invalid op\n"));
			return SE3_ERR_PARAMS;
    }

	return SE3_OK;
}

uint16_t key_find(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    if (req_size != 4) {
        SE3_TRACE(("[key_find] req size mismatch\n"));
        return SE3_ERR_PARAMS;
    }
    if (!login_struct.y) {
        SE3_TRACE(("[key_edit] not logged in\n"));
        return SE3_ERR_ACCESS;
    }
    uint32_t keyid = 0;
    se3_flash_it it = { .addr = NULL };
    SE3_GET32(req, 2, keyid); // get key ID
    // check if there is already a key with same ID
    se3_flash_it_init(&it);
    if(!se3_key_find(keyid, &it)) {
        *resp_size = 1;
        resp[0] = 1;
    } else {
        *resp_size = 1;
        resp[0] = 0;
    }
	return SE3_OK;
}

uint16_t dispatcher_call(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp)
{
    se3_cmd_func handler = NULL;
    uint16_t resp1_size, req1_size;
    uint16_t resp1_size_padded;
    const uint8_t* req1;
    uint8_t* resp1;
    uint16_t status;
    struct {
        const uint8_t* auth;
        const uint8_t* iv;
        const uint8_t* token;
        uint16_t len;
        uint16_t cmd;
        const uint8_t* data;
    } req_params;
    struct {
        uint8_t* auth;
        uint8_t* iv;
        uint8_t* token;
        uint16_t len;
        uint16_t status;
        uint8_t* data;
    } resp_params;

    req_params.auth = req + SE3_REQ1_OFFSET_AUTH;
    req_params.iv = req + SE3_REQ1_OFFSET_IV;
    req_params.token = req + SE3_REQ1_OFFSET_TOKEN;
    req_params.data = req + SE3_REQ1_OFFSET_DATA;

    uint16_t command =0;
    SE3_GET16(req, SE3_REQ1_OFFSET_CMD, command);

    if (req_size < SE3_REQ1_OFFSET_DATA) {
        SE3_TRACE(("[dispatcher_call] insufficient req size\n"));
        return SE3_ERR_COMM;
    }

    // check for authorization
    if(!sekey_get_auth(login_struct.key)){
    	return SE3_ERR_ACCESS;
    }

    // prepare request
    if (!login_struct.cryptoctx_initialized) {
        se3_payload_cryptoinit(&(login_struct.cryptoctx), login_struct.key);
        login_struct.cryptoctx_initialized = true;
    }
    if (!se3_payload_decrypt(
        &(login_struct.cryptoctx), req_params.auth, req_params.iv,
        /* !! modifying request */ (uint8_t*)(req  + SE3_AUTH_SIZE + SE3_IV_SIZE),
        (req_size - SE3_AUTH_SIZE - SE3_IV_SIZE) / SE3_CRYPTOBLOCK_SIZE, req_hdr.cmd_flags, crypto_algo))
    {
        SE3_TRACE(("[dispatcher_call] AUTH failed\n"));
        return SE3_ERR_COMM;
    }

    if (login_struct.y) {
        if (memcmp(login_struct.token, req_params.token, SE3_TOKEN_SIZE)) {
        	if (command==SE3_CMD1_CHALLENGE){//someone (maybe same user after a crash) trying to login.
				SE3_TRACE(("[dispatcher_call] login token mismatch and trying to login\n"));
				return SE3_ERR_OPENED;//notify host there is already an opened session, if host wants to continue, will call SE3_CMD1_LOGOUT_FORCED
			}
			else if (command==SE3_CMD1_LOGOUT_FORCED){//if the user agreed to close the existing session by forcing a logout
				command = SE3_CMD1_LOGOUT;//call logout as usual
				SE3_SET16(req, SE3_REQ1_OFFSET_CMD, command);
			}
			else{
				SE3_TRACE(("[dispatcher_call] login token mismatch\n"));
				return SE3_ERR_ACCESS;
			}
        }
    }

    SE3_GET16(req, SE3_REQ1_OFFSET_LEN, req_params.len);
    SE3_GET16(req, SE3_REQ1_OFFSET_CMD, req_params.cmd);

    if (req_params.cmd < SE3_CMD1_MAX) {
    	if (req_params.cmd > 6 && req_params.cmd < 11 && !login_struct.y) {
    		SE3_TRACE(("[crypto_init] not logged in\n"));
    		return SE3_ERR_ACCESS;
    	}
    	if(sekey_get_implementation_info(&algo_implementation, &crypto_algo, login_struct.key)){
    		handler = handlers[algo_implementation][req_params.cmd];
    	} else {
    		return SE3_ERR_ACCESS;
    	}
    }
    if (handler == NULL) {
        handler = error;
    }

    req1 = req_params.data;
    req1_size = req_params.len;
    resp1 = resp + SE3_RESP1_OFFSET_DATA;
    resp1_size = 0;

    status = handler(req1_size, req1, &resp1_size, resp1);

    resp_params.len = resp1_size;
    resp_params.auth = resp + SE3_RESP1_OFFSET_AUTH;
    resp_params.iv = resp + SE3_RESP1_OFFSET_IV;
    resp_params.token = resp + SE3_RESP1_OFFSET_TOKEN;
    resp_params.status = status;
    resp_params.data = resp1;

    resp1_size_padded = resp1_size;
    if (resp1_size_padded % SE3_CRYPTOBLOCK_SIZE != 0) {
        memset(resp1 + resp1_size_padded, 0, (SE3_CRYPTOBLOCK_SIZE - (resp1_size_padded % SE3_CRYPTOBLOCK_SIZE)));
        resp1_size_padded += (SE3_CRYPTOBLOCK_SIZE - (resp1_size_padded % SE3_CRYPTOBLOCK_SIZE));
    }

    *resp_size = SE3_RESP1_OFFSET_DATA + resp1_size_padded;

    // prepare response
    SE3_SET16(resp, SE3_RESP1_OFFSET_LEN, resp_params.len);
    SE3_SET16(resp, SE3_RESP1_OFFSET_STATUS, resp_params.status);
    if (login_struct.y) {
        memcpy(resp + SE3_RESP1_OFFSET_TOKEN, login_struct.token, SE3_TOKEN_SIZE);
    }
    else {
        memset(resp + SE3_RESP1_OFFSET_TOKEN, 0, SE3_TOKEN_SIZE);
    }
	if (req_hdr.cmd_flags & SE3_CMDFLAG_ENCRYPT) {
		se3_rand(SE3_IV_SIZE, resp_params.iv);
	}
	else {
		memset(resp_params.iv, 0, SE3_IV_SIZE);
	}

	// Implementation choice, dependent on the SEkey choice
	switch(algo_implementation){
		case SE3_SECURITY_CORE:
			se3_payload_encrypt(&(login_struct.cryptoctx),
								resp_params.auth,
								resp_params.iv,
								resp + SE3_AUTH_SIZE + SE3_IV_SIZE,
								(*resp_size - SE3_AUTH_SIZE - SE3_IV_SIZE) / SE3_CRYPTOBLOCK_SIZE,
								req_hdr.cmd_flags,
								crypto_algo);
			break;
		case SE3_SMARTCARD: // to be implemented (leftover from early SEcube developers)
		case SE3_FPGA: // to be implemented (leftover from early SEcube developers)
		default:
			return SE3_ERR_RESOURCE;
	}
    return SE3_OK;
}

void se3_dispatcher_init()
{
	se3_security_core_init();
    memset(&login_struct, 0, sizeof(login_struct));
    se3_security_info.records[SE3_RECORD_TYPE_USERPIN].read_access = SE3_ACCESS_MAX;
    se3_security_info.records[SE3_RECORD_TYPE_USERPIN].write_access = SE3_ACCESS_ADMIN;
    se3_security_info.records[SE3_RECORD_TYPE_ADMINPIN].read_access = SE3_ACCESS_MAX;
    se3_security_info.records[SE3_RECORD_TYPE_ADMINPIN].write_access = SE3_ACCESS_ADMIN;
    se3_mem_init(&(se3_security_info.sessions), SE3_SESSIONS_MAX, se3_sessions_index, SE3_SESSIONS_BUF, se3_sessions_buf);
    login_cleanup();
}

void set_req_hdr(se3_comm_req_header req_hdr_i){
	req_hdr = req_hdr_i;
}

static void login_cleanup()
{
    size_t i;
    se3_mem_reset(&(se3_security_info.sessions));
    login_struct.y = false;
    login_struct.access = 0;
    login_struct.challenge_access = SE3_ACCESS_MAX;
    login_struct.cryptoctx_initialized = false;
    memcpy(login_struct.key, se3_magic, SE3_KEY_SIZE);
    memset(login_struct.token, 0, SE3_TOKEN_SIZE);
    for (i = 0; i < SE3_SESSIONS_MAX; i++) {
        se3_security_info.sessions_algo[i] = SE3_ALGO_INVALID;
    }
}
