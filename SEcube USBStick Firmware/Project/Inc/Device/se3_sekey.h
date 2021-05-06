/**
  ******************************************************************************
  * File Name          : se3_sekey.h
  * Description        : Firmware functionalities to support the KMS
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

#ifndef SE3_SEKEY_H
#define SE3_SEKEY_H

#include "se3_keys.h"
#include <stdint.h>
#include <stdbool.h>

#include "se3_security_core.h"

#define SE3_TYPE_USERINFO 117 /**< This is the identifier of a flash node that contains the name and the ID of a SEkey user. */

extern enum {
	SE3_SECURITY_CORE,
	SE3_FPGA,
	SE3_SMARTCARD
}se3_algo_impl_t;

enum{
	SE3_SEKEY_OP_SETINFO = 1, /**< Store SEkey username and user ID in the flash memory. */
	SE3_SEKEY_OP_GETINFO = 2, /**< Retrieve SEkey username and user ID from the flash memory. */
	SE3_SEKEY_OP_GET_KEY_IDS = 4, /**< Retrieve the IDs of all keys stored in the flash memory. */
	SE3_SEKEY_DELETEALL = 5, /**< Delete all keys stored in the flash memory. */
	SE3_SEKEY_DELETEKEY = 6, /**< Delete a specific key from the flash memory. */
	SE3_SEKEY_OP_GETKEYENC = 7, /**< Retrieve the value of a key as ciphertext from the flash memory. */
	SE3_SEKEY_INSERTKEY = 8, /**< Store a new key in the flash memory. */
	SE3_SEKEY_ISREADY = 9 /**< Check if the SEcube is ready to be used with SEkey. */
};

/** \brief Retrieve the ID and length of the keys stored on the SEcube.
 * This function iterates over the entire flash memory reading the ID and length of each stored key. These data are written
 * into the response buffer. Since there is a maximum amount of bytes that can be sent in the communication back to the host
 * PC, this function may be called multiple times in a row. Each time it is called, it will start searching for keys from the
 * last one that was found. When all the keys have been scanned, the function will insert a set of 0s into the response buffer
 * so that the host PC will know that the key ID and length retrieval process has ended.
 * Notice that some keys, depending on the request coming from the host, may be skipped. This function, in fact, is used both
 * by manually administered keys and by the SEkey KMS, therefore a flag determines if the keys that need to be returned are the
 * one for manual management or automatic management (KMS). */
uint16_t load_key_ids(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Send to the host the value of a key as ciphertext.
 * This function is required by the SEkey administrator in order to distribute the keys to the users.
 * This function will not work if the active login on the SEcube is not in administrator mode. The value of the key passed
 * in the request buffer will be written as ciphertext in the response buffer, provided that a single key with that ID is
 * stored in the flash memory. In case of multiple keys with the same ID, an error is returned. In order to encrypt the key
 * to be returned, another key present in the flash memory has to be specified by the host in the request buffer. */
uint16_t load_encrypted_key_data(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Send the name and the ID of the SEkey user associated to the SEcube to the host computer. */
uint16_t load_user_info(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Write the name and the ID of the SEkey user associated to the SEcube into the flash memory.
 * This action is performed only by the SEkey administrator therefore it is not allowed when the active login
 * is in user mode. */
uint16_t store_user_info(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Delete all the keys stored in the flash memory of the SEcube.
 * The host can specify a list of IDs, inside the request buffer, not to be deleted in order to
 * preserve them inside the flash memory. This is useful for SEkey. */
uint16_t delete_all_keys(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Checks if the SEcube is ready to be used with SEkey. */
uint16_t is_ready(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Simply delete a key from the flash memory. The ID of the key to be deleted is passed in the request buffer. */
uint16_t delete_key(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief Store a key inside the flash memory of the SEcube.
 *
 * The request buffer may or may not contain the value of the key to be stored. In case the value is specified in the
 * request buffer, it may be encrypted or not. If it is encrypted, the ID of the key to be used for decryption is
 * also specified. If the value of the key is not found in the request buffer, then the encryption key is generated
 * using  the TRNG embedded in the SEcube. If another key with the same id is already stored in the SEcube, the old key is replaced by the new one.
 */
uint16_t insert_key(uint16_t req_size, const uint8_t* req, uint16_t* resp_size, uint8_t* resp);

/** \brief SEkey behaviour function
 *  Retrieve information, given a key, about the possible implementations you're allowed to perform, and the algorithm.
 *  Warning: this is a leftover from the early developers. Probably to be removed.
 */
bool sekey_get_implementation_info(uint8_t* algo_implementation, uint8_t* crypto_algo, uint8_t* key);

/** \brief SEkey checking on keys
 *  Checks whether the passed key is registered into SEkey.
 *  Warning: this is a leftover from the early developers. Probably to be removed.
 */
bool sekey_get_auth(uint8_t *key);

#endif
