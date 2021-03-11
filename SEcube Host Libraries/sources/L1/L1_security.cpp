/**
  ******************************************************************************
  * File Name          : L1_security.cpp
  * Description        : Implementation of the SECURITY API.
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
 * @brief	Implementation of the SECURITY API
 *
 * The file contains the implementation of the SECURITY API
 */

#include "L1.h"
#include "L1_error_manager.h"

using namespace std;

//#define SIGNATURE_DEBUG // enable this to debug the value of signatures verified in L1Decrypt

void L1::L1CryptoInit(uint16_t algorithm, uint16_t mode, uint32_t keyId, uint32_t& sessId) {
	uint8_t* _algo = (uint8_t*)&algorithm;
	uint8_t* _mode = (uint8_t*)&mode;
	uint8_t* _keyId = (uint8_t*)&keyId;
	uint16_t respLen = 0;

	this->base.FillSessionBuffer(_algo,	L1Response::Offset::DATA + L1Crypto::InitRequestOffset::ALGO, 2);
	this->base.FillSessionBuffer(_mode,	L1Response::Offset::DATA + L1Crypto::InitRequestOffset::MODE, 2);
	this->base.FillSessionBuffer(_keyId, L1Response::Offset::DATA + L1Crypto::InitRequestOffset::KEY_ID, 4);

	L1CryptoInitException cryptoInitExc;
	try {
		TXRXData(L1Commands::Codes::CRYPTO_INIT, L1Crypto::InitRequestSize::SIZE, 0, &respLen); //send the data

	}
	catch(L1Exception& e) {
		throw cryptoInitExc;
	}

	uint32_t u32Tmp = 0;
	this->base.ReadSessionBuffer((uint8_t*)&u32Tmp, L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::SID, 4);
	sessId = u32Tmp;
}

void L1::L1CryptoUpdate(uint32_t sessId, uint16_t flags, uint16_t data1Len, uint8_t* data1, uint16_t data2Len, uint8_t* data2, uint16_t* dataOutLen, uint8_t* dataOut) {
	if(data1Len == 0 && data2Len == 0){
		throw std::invalid_argument("Cannot pass empty input buffers!");
	}

	uint8_t* _sessId = (uint8_t*)&sessId;
	uint8_t* _flags = (uint8_t*)&flags;
	uint8_t* _data1Len = (uint8_t*)&data1Len;
	uint8_t* _data2Len = (uint8_t*)&data2Len;

	this->base.FillSessionBuffer(_sessId, L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::SID, 4);
	this->base.FillSessionBuffer(_flags, L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::FLAGS, 2);
	this->base.FillSessionBuffer(_data1Len,	L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::DATAIN1_LEN, 2);
	this->base.FillSessionBuffer(_data2Len,	L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::DATAIN2_LEN, 2);

	//compute the offset for Data2
	uint16_t data1LenPadded = 0;
	if((data1Len % 16) != 0){ // is data1Len not a multiple of 16?
		data1LenPadded = data1Len + (16 - (data1Len % 16)); // if it's not then wrap it
	} else {
		data1LenPadded = data1Len; // if it is then just assign it
	}

	// check if the buffer length is exceeded
	L1CryptoUpdateException cryptoUpdateExc;
	uint16_t dataLen = L1Crypto::UpdateRequestOffset::DATA + data1LenPadded + data2Len;
	if (dataLen > L1Request::Size::MAX_DATA){
		throw cryptoUpdateExc;
	}

	// fill the buffer with data1
	if ((data1Len > 0) && (data1 != nullptr)){
		this->base.FillSessionBuffer(data1,	L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::DATA,	data1Len);
	}
	// fill the buffer with data2
	if ((data2Len > 0) && (data2 != nullptr)){
		this->base.FillSessionBuffer(data2,	L1Response::Offset::DATA + L1Crypto::UpdateRequestOffset::DATA + data1LenPadded, data2Len);
	}

	//send the data
	uint16_t respLen;
	try {
		TXRXData(L1Commands::Codes::CRYPTO_UPDATE, dataLen, 0, &respLen);
	}
	catch(L1Exception& e) {
		throw cryptoUpdateExc;
	}

	uint16_t u16tmp;
	this->base.ReadSessionBuffer((uint8_t*)&u16tmp,	L1Response::Offset::DATA + L1Crypto::UpdateResponseOffset::DATAOUT_LEN,	2);	//extract the data length
	if(dataOutLen != nullptr){
		*dataOutLen = u16tmp;
	}
	if(dataOut != nullptr){
		//extract the data
		memcpy(dataOut, this->base.GetSessionBuffer() + L1Response::Offset::DATA + L1Crypto::UpdateResponseOffset::DATA, u16tmp);
	}
}

void L1::L1Encrypt(size_t plaintext_size, std::shared_ptr<uint8_t[]> plaintext, SEcube_ciphertext& encrypted_data, uint16_t algorithm, uint16_t algorithm_mode, uint32_t key_id) {
	L1EncryptException encryptExc;
	if(plaintext == nullptr){
		throw encryptExc;
	}
	if((algorithm == L1Algorithms::Algorithms::HMACSHA256) || (algorithm == L1Algorithms::Algorithms::SHA256)){
		throw std::invalid_argument("Cannot call L1Encrypt with digest algorithms. Call L1Digest instead.");
	}
	if((algorithm != L1Algorithms::Algorithms::AES) && (algorithm != L1Algorithms::Algorithms::AES_HMACSHA256)){
		throw std::invalid_argument("Invalid algorithm.");
	}
	if((algorithm_mode != CryptoInitialisation::Modes::ECB) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CBC) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CTR) &&
	   (algorithm_mode != CryptoInitialisation::Modes::OFB) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CFB)){
		throw std::invalid_argument("Invalid algorithm mode.");
	}
	uint32_t encSessId = 0; // crypto session identifier on the SEcube
	uint16_t curr_len = 0; // how many bytes have been processed by the latest cryptoupdate() call
	size_t curr_chunk = 0; // dimension (bytes) of current portion of data to be encrypted
	uint64_t enc_size = 0; // counter of how many bytes are encrypted
	size_t total_size_copy = 0; // here we will store a copy of the total size of the plaintext
	unique_ptr<uint8_t[]> ciphertext; // here we will store the encrypted data
	encrypted_data.reset(); // reset content of the L1Ciphertext object (in case the caller provided an object already used before)
	encrypted_data.algorithm = algorithm;
	encrypted_data.mode = algorithm_mode;
	encrypted_data.key_id = key_id;
	try {
		L1CryptoInit(algorithm, algorithm_mode | CryptoInitialisation::Direction::ENCRYPT, key_id, encSessId);
		uint8_t padding = (B5_AES_BLK_SIZE - (plaintext_size % B5_AES_BLK_SIZE)); // PKCS#7 padding
		size_t total_size = plaintext_size + padding;
		total_size_copy = total_size;
		unique_ptr<uint8_t[]> plaintext_padded = make_unique<uint8_t[]>(total_size);
		memcpy(plaintext_padded.get(), plaintext.get(), plaintext_size); // copy first part (real plaintext)
		memset(plaintext_padded.get()+plaintext_size, padding, padding); // setup padding value (this doesn't do anything if padding=0)
		ciphertext = make_unique<uint8_t[]>(total_size); // by default allocated for simple AES

		if(algorithm_mode == CryptoInitialisation::Modes::CTR){
			enum{
				/* we need this new value because, in CTR mode, the L1CryptoUpdate will be called passing both the cleartext and the counter,
				 * so we have to consider also the size of the nonce when sending the cryptoupdate. */
				DATAIN_NEW = L1Crypto::UpdateSize::DATAIN - B5_AES_BLK_SIZE
			};
			curr_chunk = total_size < DATAIN_NEW ? total_size : DATAIN_NEW; // by default computed for simple AES
			uint8_t ctr_nonce[B5_AES_BLK_SIZE]; // nonce required by AES-CTR (it is combined with the counter)
			memset(ctr_nonce, 0, B5_AES_BLK_SIZE);
			L0Support::Se3Rand(B5_AES_BLK_SIZE, ctr_nonce); // fill nonce with random bytes
			uint64_t ctr_counter = 0;
			memcpy(ctr_nonce+8, &ctr_counter, 8); // nonce is made of first 64 bits that are fixed (actual nonce) and last 64 bits that are the counter
			// NONCE -> 64 random bits (real nonce) concatenated 64 bits (counter)
			// NONCE: uint64_t counter allows for 2^64 * 16 bytes of data...plenty enough
			for(int i=0; i<B5_AES_BLK_SIZE; i++){ // copy ctr_nonce to L1Ciphertext object
				encrypted_data.CTR_nonce.at(i) = ctr_nonce[i];
			}
			if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES + HMAC-SHA256
				ciphertext.reset();
				ciphertext = make_unique<uint8_t[]>(total_size+B5_SHA256_DIGEST_SIZE); // reassign including signature
				curr_chunk = total_size < (DATAIN_NEW - B5_SHA256_DIGEST_SIZE) ? total_size : (DATAIN_NEW - B5_SHA256_DIGEST_SIZE); // recomputed including signature
				uint8_t nonce_key_derivation_hmac_sha256[32]; // nonce to derive (with pbdkf2) the key used to authenticate the digest with HMAC-SHA256
				memset(nonce_key_derivation_hmac_sha256, 0, 32);
				L0Support::Se3Rand(32, nonce_key_derivation_hmac_sha256); // fill nonce with random bytes
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, nonce_key_derivation_hmac_sha256, 0, nullptr, nullptr, nullptr); // set nonce for HMAC-SHA256 key derivation
				for(int i=0; i<32; i++){ // copy nonce to L1Ciphertext object
					encrypted_data.digest_nonce.at(i) = nonce_key_derivation_hmac_sha256[i];
				}
			}
			uint8_t *decrypted = plaintext_padded.get(); // alias for plaintext
			uint8_t *encrypted = ciphertext.get(); // alias for ciphertext
			do {
				if(total_size - curr_chunk){ // still in the middle of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, decrypted, &curr_len, encrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, decrypted, &curr_len, encrypted);
					}
				}
				else{ // last chunk of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET | L1Crypto::UpdateFlags::AUTH | L1Crypto::UpdateFlags::FINIT, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, decrypted, &curr_len, encrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::FINIT, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, decrypted, &curr_len, encrypted);
					}
				}
				ctr_counter++; // increment counter and concatenate it with the nonce
				memcpy(ctr_nonce+8, &ctr_counter, 8);
				total_size -= curr_chunk; // decrement total number of bytes
				decrypted += curr_chunk; // move on pointer of plaintext
				encrypted += curr_chunk; // move on pointer of ciphertext
				curr_chunk = total_size < DATAIN_NEW ? total_size : DATAIN_NEW; // next chunk of data
				if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // recomputed including signature
					curr_chunk = total_size < (DATAIN_NEW - B5_SHA256_DIGEST_SIZE) ? total_size : (DATAIN_NEW - B5_SHA256_DIGEST_SIZE);
				}
				enc_size += curr_len; // increment counter of encrypted data
			} while(total_size > 0);
			if((total_size_copy != enc_size) && (total_size_copy != (enc_size - 32))){
				throw encryptExc; // throw exception if total encoded size is different from the original size (include also signature case with AES-HMAC-SHA256)
			}
		} else {
			curr_chunk = total_size < L1Crypto::UpdateSize::DATAIN ? total_size : L1Crypto::UpdateSize::DATAIN; // by default computed for simple AES
			if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){
				ciphertext.reset();
				ciphertext = make_unique<uint8_t[]>(total_size+B5_SHA256_DIGEST_SIZE); // reassign including signature
				curr_chunk = total_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? total_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE); // recomputed including signature
				uint8_t nonce_key_derivation_hmac_sha256[32]; // nonce to derive (with pbdkf2) the key used to authenticate the digest with HMAC-SHA256
				memset(nonce_key_derivation_hmac_sha256, 0, 32);
				L0Support::Se3Rand(32, nonce_key_derivation_hmac_sha256); // fill nonce with random bytes
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, nonce_key_derivation_hmac_sha256, 0, nullptr, nullptr, nullptr); // set nonce for HMAC-SHA256 key derivation
				for(int i=0; i<32; i++){ // copy nonce to L1Ciphertext object
					encrypted_data.digest_nonce.at(i) = nonce_key_derivation_hmac_sha256[i];
				}
			}
			if((algorithm_mode == CryptoInitialisation::Modes::CBC) ||
			   (algorithm_mode == CryptoInitialisation::Modes::CFB) ||
			   (algorithm_mode == CryptoInitialisation::Modes::OFB)){
				uint8_t aes_initialization_vector[B5_AES_BLK_SIZE]; // initialization vector
				memset(aes_initialization_vector, 0, B5_AES_BLK_SIZE);
				L0Support::Se3Rand(B5_AES_BLK_SIZE, aes_initialization_vector); // fill IV with random bytes
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SET_IV, B5_AES_BLK_SIZE, aes_initialization_vector, 0, nullptr, nullptr, nullptr); // set IV
				for(int i=0; i<B5_AES_BLK_SIZE; i++){ // copy IV to L1Ciphertext object
					encrypted_data.initialization_vector.at(i) = aes_initialization_vector[i];
				}
			}
			uint8_t *decrypted = plaintext_padded.get(); // alias for plaintext
			uint8_t *encrypted = ciphertext.get(); // alias for ciphertext
			do {
				if(total_size - curr_chunk){ // still in the middle of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, 0, 0, nullptr, curr_chunk, decrypted, &curr_len, encrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, 0, 0, nullptr, curr_chunk, decrypted, &curr_len, encrypted);
					}
				}
				else{ // last chunk of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET | L1Crypto::UpdateFlags::AUTH | L1Crypto::UpdateFlags::FINIT, 0, nullptr, curr_chunk, decrypted, &curr_len, encrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::FINIT, 0, nullptr, curr_chunk, decrypted, &curr_len, encrypted);
					}
				}
				total_size -= curr_chunk; // decrement total number of bytes
				decrypted += curr_chunk; // move on pointer of plaintext
				encrypted += curr_chunk; // move on pointer of ciphertext
				curr_chunk = total_size < L1Crypto::UpdateSize::DATAIN ? total_size : L1Crypto::UpdateSize::DATAIN; // next chunk of data
				if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // recomputed including signature
					curr_chunk = total_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? total_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE);
				}
				enc_size += curr_len; // increment counter of encrypted data
			} while(total_size > 0);
			if((total_size_copy != enc_size) && (total_size_copy != (enc_size - 32))){
				throw encryptExc; // throw exception if total encoded size is different from the original size (include also signature case with AES-HMAC-SHA256)
			}
		}

		// encryption is done, copy data into L1Ciphertext object
		encrypted_data.ciphertext = make_unique<uint8_t[]>(total_size_copy);
		memcpy(encrypted_data.ciphertext.get(), ciphertext.get(), total_size_copy); // copy encrypted data
		encrypted_data.ciphertext_size = total_size_copy; // copy encrypted data size
		if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){
			for(int i=0; i<B5_SHA256_DIGEST_SIZE; i++){
				encrypted_data.digest.at(i) = ciphertext[i+total_size_copy]; // copy AES-HMAC-SHA256 digest
			}
		}
	}
	catch (L1Exception& e) {
		throw encryptExc;
	}
}

void L1::L1Decrypt(SEcube_ciphertext& encrypted_data, size_t& plaintext_size, std::shared_ptr<uint8_t[]>& plaintext) {
	L1DecryptException decryptExc;
	uint16_t algorithm = encrypted_data.algorithm;
	uint16_t algorithm_mode = encrypted_data.mode;
	uint32_t key_id = encrypted_data.key_id;
	if((algorithm == L1Algorithms::Algorithms::HMACSHA256) || (algorithm == L1Algorithms::Algorithms::SHA256)){
		throw std::invalid_argument("Cannot call L1Decrypt with digest algorithms. Call L1Digest instead.");
	}
	if((algorithm != L1Algorithms::Algorithms::AES) && (algorithm != L1Algorithms::Algorithms::AES_HMACSHA256)){
		throw std::invalid_argument("Invalid algorithm.");
	}
	if((algorithm_mode != CryptoInitialisation::Modes::ECB) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CBC) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CTR) &&
	   (algorithm_mode != CryptoInitialisation::Modes::OFB) &&
	   (algorithm_mode != CryptoInitialisation::Modes::CFB)){
		throw std::invalid_argument("Invalid algorithm mode.");
	}
	uint32_t encSessId = 0; // crypto session identifier on the SEcube
	uint16_t curr_len = 0; // how many bytes have been processed by the latest cryptoupdate() call
	size_t curr_chunk = 0; // dimension (bytes) of current portion of data to be decrypted
	uint64_t dec_size = 0; // counter of how many bytes are decrypted
	size_t enc_size = encrypted_data.ciphertext_size; // here we will store a copy of the size of the ciphertext
	unique_ptr<uint8_t[]> decrypted_data; // here we will store the decrypted data
	try {
		L1CryptoInit(algorithm, algorithm_mode | CryptoInitialisation::Direction::DECRYPT, key_id, encSessId);
		decrypted_data  = make_unique<uint8_t[]>(encrypted_data.ciphertext_size); // by default allocated for simple AES
		if(algorithm_mode == CryptoInitialisation::Modes::CTR){
			enum{
				/* we need this new value because, in some cases, the L1CryptoUpdate will be called passing both the cleartext and the nonce,
				 * so we have to consider also the size of the nonce when sending the cryptoupdate. */
				DATAIN_NEW = L1Crypto::UpdateSize::DATAIN - B5_AES_BLK_SIZE
			};
			curr_chunk = encrypted_data.ciphertext_size < DATAIN_NEW ? encrypted_data.ciphertext_size : DATAIN_NEW; // by default computed for simple AES
			uint8_t ctr_nonce[B5_AES_BLK_SIZE]; // nonce required by AES-CTR (it is combined with the counter)
			memcpy(ctr_nonce, encrypted_data.CTR_nonce.data(), B5_AES_BLK_SIZE);
			uint64_t ctr_counter = 0;
			memcpy(ctr_nonce+8, &ctr_counter, 8); // nonce is made of first 64 bits that are fixed (actual nonce) and last 64 bits that are the counter
			// NONCE -> 64 random bits (real nonce) concatenated 64 bits (counter)
			// NONCE: uint64_t counter allows for 2^64 * 16 bytes of data...plenty enough
			if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES + HMAC-SHA256
				decrypted_data.reset();
				decrypted_data = make_unique<uint8_t[]>(encrypted_data.ciphertext_size+B5_SHA256_DIGEST_SIZE); // reassign including signature
				curr_chunk = encrypted_data.ciphertext_size < (DATAIN_NEW - B5_SHA256_DIGEST_SIZE) ? encrypted_data.ciphertext_size : (DATAIN_NEW - B5_SHA256_DIGEST_SIZE); // recomputed including signature
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, encrypted_data.digest_nonce.data(), 0, nullptr, nullptr, nullptr); // set nonce for HMAC-SHA256 key derivation
			}
			uint8_t *decrypted = decrypted_data.get(); // alias for plaintext
			uint8_t *encrypted = encrypted_data.ciphertext.get(); // alias for ciphertext
			do {
				if(enc_size - curr_chunk){ // still in the middle of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, encrypted, &curr_len, decrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, encrypted, &curr_len, decrypted);
					}
				}
				else{ // last chunk of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET | L1Crypto::UpdateFlags::AUTH | L1Crypto::UpdateFlags::FINIT, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, encrypted, &curr_len, decrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::FINIT, B5_AES_BLK_SIZE, ctr_nonce, curr_chunk, encrypted, &curr_len, decrypted);
					}
				}
				ctr_counter++; // increment counter and concatenate it with the nonce
				memcpy(ctr_nonce+8, &ctr_counter, 8);
				enc_size -= curr_chunk; // decrement total number of bytes
				decrypted += curr_chunk; // move on pointer of plaintext
				encrypted += curr_chunk; // move on pointer of ciphertext
				curr_chunk = enc_size < DATAIN_NEW ? enc_size : DATAIN_NEW; // next chunk of data
				if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // recomputed including signature
					curr_chunk = enc_size < (DATAIN_NEW - B5_SHA256_DIGEST_SIZE) ? enc_size : (DATAIN_NEW - B5_SHA256_DIGEST_SIZE);
				}
				dec_size += curr_len; // increment counter of encrypted data
			} while(enc_size > 0);
		} else {
			curr_chunk = encrypted_data.ciphertext_size < L1Crypto::UpdateSize::DATAIN ? encrypted_data.ciphertext_size : L1Crypto::UpdateSize::DATAIN; // by default computed for simple AES
			if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){
				decrypted_data.reset();
				decrypted_data = make_unique<uint8_t[]>(encrypted_data.ciphertext_size+B5_SHA256_DIGEST_SIZE); // reassign including signature
				curr_chunk = encrypted_data.ciphertext_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? encrypted_data.ciphertext_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE); // recomputed including signature
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, encrypted_data.digest_nonce.data(), 0, nullptr, nullptr, nullptr); // set nonce for HMAC-SHA256 key derivation
			}
			if((algorithm_mode == CryptoInitialisation::Modes::CBC) ||
			   (algorithm_mode == CryptoInitialisation::Modes::CFB) ||
			   (algorithm_mode == CryptoInitialisation::Modes::OFB)){
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SET_IV, B5_AES_BLK_SIZE, encrypted_data.initialization_vector.data(), 0, nullptr, nullptr, nullptr); // set IV
			}
			uint8_t *decrypted = decrypted_data.get(); // alias for plaintext
			uint8_t *encrypted = encrypted_data.ciphertext.get(); // alias for ciphertext
			do {
				if(enc_size - curr_chunk){ // still in the middle of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, 0, 0, nullptr, curr_chunk, encrypted, &curr_len, decrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, 0, 0, nullptr, curr_chunk, encrypted, &curr_len, decrypted);
					}
				}
				else{ // last chunk of data
					if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // AES with HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::RESET | L1Crypto::UpdateFlags::AUTH | L1Crypto::UpdateFlags::FINIT, 0, nullptr, curr_chunk, encrypted, &curr_len, decrypted);
					} else { // standard AES without HMAC-SHA256
						L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::FINIT, 0, nullptr, curr_chunk, encrypted, &curr_len, decrypted);
					}
				}
				enc_size -= curr_chunk; // decrement total number of bytes
				decrypted += curr_chunk; // move on pointer of plaintext
				encrypted += curr_chunk; // move on pointer of ciphertext
				curr_chunk = enc_size < L1Crypto::UpdateSize::DATAIN ? enc_size : L1Crypto::UpdateSize::DATAIN; // next chunk of data
				if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // recomputed including signature
					curr_chunk = enc_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? enc_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE);
				}
				dec_size += curr_len; // increment counter of decrypted data
			} while(enc_size > 0);
		}
		if((encrypted_data.ciphertext_size != dec_size) && (encrypted_data.ciphertext_size != (dec_size - 32))){
			throw decryptExc; // throw exception if total decrypted size is different from the original size (include also signature case with AES-HMAC-SHA256)
		}
		uint8_t padding_size = 0;
		uint64_t decrypted_size = dec_size;
		if(algorithm == L1Algorithms::Algorithms::AES_HMACSHA256){ // check signature
			L1DataIntegrityException exc;
			int r = memcmp(encrypted_data.digest.data(), decrypted_data.get()+dec_size-B5_SHA256_DIGEST_SIZE, B5_SHA256_DIGEST_SIZE);
#ifdef SIGNATURE_DEBUG
			cout << "\n\nSignature attached to ciphertext:" << endl;
			for(uint8_t val : encrypted_data.digest){
				printf("%02x ", val);
			}
			cout << "\n\nSignature recomputed on decrypted data:" << endl;
			for(int i=0; i<B5_SHA256_DIGEST_SIZE; i++){
				printf("%02x ", decrypted_data[dec_size-B5_SHA256_DIGEST_SIZE+i]); // signature is at the end
			}
			if(r){
				cout << "\nsignatures do not match!" << endl;
			} else {
				cout << "\nsignatures match!" << endl;
			}
#endif
			if(r){ // signature does not match
				throw exc;
			}
			padding_size = decrypted_data[dec_size-B5_SHA256_DIGEST_SIZE-1];
			decrypted_size -= (padding_size + B5_SHA256_DIGEST_SIZE);
		} else {
			padding_size = decrypted_data[dec_size-1];
			decrypted_size -= padding_size;
		}
		shared_ptr<uint8_t[]> tmp(new uint8_t[decrypted_size]);
		memcpy(tmp.get(), decrypted_data.get(), decrypted_size);
		plaintext_size = decrypted_size;
		plaintext.swap(tmp);
	}
	catch(L1Exception& e) {
		throw decryptExc;
	}
}

void L1::L1Digest(size_t input_size, std::shared_ptr<uint8_t[]> input_data, SEcube_digest& digest) {
	L1DigestException digestExc;
	if(((digest.algorithm != L1Algorithms::Algorithms::HMACSHA256) && (digest.algorithm != L1Algorithms::Algorithms::SHA256))){
		throw digestExc;
	}
	uint32_t encSessId = 0;
	std::unique_ptr<uint8_t[]> output_data = make_unique<uint8_t[]>(32);
	uint8_t *input = input_data.get(); // alias for input data
	uint8_t *output = output_data.get(); // alias for digest
	try {
		switch(digest.algorithm){
			case L1Algorithms::Algorithms::HMACSHA256:
				L1CryptoInit(digest.algorithm, 0, digest.key_id, encSessId);
				if(digest.usenonce){
					L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, digest.digest_nonce.data(), 0, nullptr, nullptr, nullptr);
				} else {
					uint8_t nonce_key_derivation_hmac_sha256[32];
					memset(nonce_key_derivation_hmac_sha256, 0, 32);
					L0Support::Se3Rand(32, nonce_key_derivation_hmac_sha256);
					L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::SETNONCE, 32, nonce_key_derivation_hmac_sha256, 0, nullptr, nullptr, nullptr);
					for(int i=0; i<32; i++){ // copy nonce to Digest object
						digest.digest_nonce.at(i) = nonce_key_derivation_hmac_sha256[i];
					}
				}
				break;
			case L1Algorithms::Algorithms::SHA256:
				L1CryptoInit(digest.algorithm, 0, L1Key::Id::NULL_ID, encSessId);
				break;
			default:
				throw digestExc;
		}
		size_t curr_chunk = input_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? input_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE);
		uint16_t curr_len = 0;
		do {
			if(input_size - curr_chunk){ // still in the middle of data
				L1CryptoUpdate(encSessId, 0, curr_chunk, input, 0, nullptr, &curr_len, output);
			}
			else{ // last chunk of data
				L1CryptoUpdate(encSessId, L1Crypto::UpdateFlags::FINIT, curr_chunk, input, 0, nullptr, &curr_len, output);
			}
			input_size -= curr_chunk;
			output += curr_chunk;
			input += curr_chunk;
			curr_chunk = input_size < (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE) ? input_size : (L1Crypto::UpdateSize::DATAIN - B5_SHA256_DIGEST_SIZE);
		} while(input_size > 0);
		for(int i=0; i<32; i++){ // copy the digest
			digest.digest[i] = output_data[i];
		}
	}
	catch (L1Exception& e) {
		throw digestExc;
	}
}

void L1::L1GetAlgorithms(std::vector<se3Algo>& algorithmsArray) {
	L1GetAlgorithmsException algoExc;
	uint16_t respLen = 0;
	uint16_t nAlgo = 0; // number of returned algorithms
	size_t offsetAlgo = L1Crypto::ListResponseOffset::ALGORITHM_INFO;
	algorithmsArray.clear();
	try {
		TXRXData(L1Commands::Codes::CRYPTO_LIST, L1Crypto::ListRequestSize::REQ_SIZE, 0, &respLen); // send request
		if(respLen < 2){ // error, we need at least the number of algorithms that are supported (at least the SEcube should reply with 0)
			throw algoExc;
		}
		this->base.ReadSessionBuffer((uint8_t*)&nAlgo, L1Response::Offset::DATA + L1Crypto::ListResponseOffset::COUNT, 2); // read response
		for(uint8_t i=0; i < nAlgo; i++){
			se3Algo currAlgo;
			this->base.ReadSessionBuffer(currAlgo.name, L1Response::Offset::DATA + offsetAlgo + L1Crypto::AlgorithmInfoOffset::NAME, L1Crypto::AlgorithmInfoSize::NAME_SIZE);
			this->base.ReadSessionBuffer((uint8_t*)&(currAlgo.type), L1Response::Offset::DATA + offsetAlgo + L1Crypto::AlgorithmInfoOffset::TYPE, 2);
			this->base.ReadSessionBuffer((uint8_t*)&(currAlgo.blockSize), L1Response::Offset::DATA + offsetAlgo + L1Crypto::AlgorithmInfoOffset::BLOCK_SIZE, 2);
			this->base.ReadSessionBuffer((uint8_t*)currAlgo.keySize, L1Response::Offset::DATA + offsetAlgo + L1Crypto::AlgorithmInfoOffset::KEY_SIZE, L1Crypto::AlgorithmInfoSize::KEY_SIZE);
			offsetAlgo += L1Crypto::AlgorithmInfoSize::SIZE;
			algorithmsArray.push_back(currAlgo);
		}
	}
	catch(L1Exception& e) {
		throw algoExc;
	}
}

void L1::L1SetAdminPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) {
	L1Exception exc;
	if(this->L1GetAccessType() != SE3_ACCESS_ADMIN){
		throw exc;
	}
	return L1Config(L1Configuration::RecordType::ADMINPIN, L1Configuration::Operation::SET, pin);
}

void L1::L1SetUserPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) {
	L1Exception exc;
	if(this->L1GetAccessType() != SE3_ACCESS_ADMIN){
		throw exc;
	}
	return L1Config(L1Configuration::RecordType::USERPIN, L1Configuration::Operation::SET, pin);
}

void L1::L1KeyEdit(se3Key& k, uint16_t op) {
	L1KeyEditException keyEditExc;
	uint16_t dataLen = 0;
	uint16_t respLen = 0;
	// check key id validity
    if((k.id == L1Key::Id::NULL_ID) ||
       (k.id == L1Key::Id::ZERO_ID) ||
	   (k.id >= L1Key::Id::SEKEY_ID_BEGIN && k.id <= L1Key::Id::SEKEY_ID_END) ||
	   (k.id >= L1Key::Id::RESERVED_ID_SEKEY_BEGIN && k.id <= L1Key::Id::RESERVED_ID_SEKEY_END)){
    	throw keyEditExc;
    }
	this->base.FillSessionBuffer((uint8_t*)&op, L1Response::Offset::DATA + L1Request::KeyOffset::OP, 2);
	dataLen += 2;
	this->base.FillSessionBuffer((uint8_t*)&(k.id), L1Response::Offset::DATA + L1Request::KeyOffset::ID, 4);
	dataLen += 4;
	this->base.FillSessionBuffer((uint8_t*)&(k.dataSize), L1Response::Offset::DATA + L1Request::KeyOffset::DATA_LEN, 2);
	dataLen += 2;
	if(op == L1Commands::KeyOpEdit::SE3_KEY_OP_ADD){
		if(k.data == nullptr){
			throw keyEditExc;
		}
		this->base.FillSessionBuffer(k.data, L1Response::Offset::DATA + L1Request::KeyOffset::DATA, k.dataSize);
		dataLen += k.dataSize;
	}
	try {
		TXRXData(L1Commands::Codes::KEY_EDIT, dataLen, 0, &respLen);
	}
	catch(L1Exception& e) {
		throw keyEditExc;
	}
}

void L1::L1KeyList(std::vector<std::pair<uint32_t, uint16_t>>& keylist){
	L1KeyListException keyListExc;
	keylist.clear();
	uint16_t resp_len = 0;
	/* since there is a precise limit to the amount of data that the host and the SEcube can exchange as
	 * request and response to a command, this function will issue multiple times the same command in order
	 * to retrieve the IDs of all the keys on the device. the iteration is required because the IDs to be returned
	 * (each one needs 4 B) may surpass the maximum size of the single response that the SEcube can send. */
	unique_ptr<uint8_t[]> buffer = make_unique<uint8_t[]>(L1Response::Size::MAX_DATA);
	for(;;){
		// send command to SEcube
		uint16_t empty = 0;
		this->base.FillSessionBuffer((uint8_t*)&empty, L1Request::Offset::DATA + L1Request::KeyOffset::OP, 2);
		uint8_t filter = 1; // enable filter on IDs in the firmware
		this->base.FillSessionBuffer((uint8_t*)&filter, L1Response::Offset::DATA + 2, 1);
		try{
			TXRXData(L1Commands::Codes::KEY_LIST, 3, 0, &resp_len);
		} catch(L1Exception& e){
			keylist.clear();
			throw keyListExc;
		}
		// copy response to local buffer
		memset(buffer.get(), 0, L1Response::Size::MAX_DATA);
		memcpy(buffer.get(), (this->base.GetSessionBuffer()+L1Request::Offset::DATA), resp_len);
		if(resp_len == 0){ // if the response is empty, the SEcube has returned all the IDs in its flash memory
			return;
		}
		if(resp_len % 6){ // if a response is not a multiple of 6 there was some problem (because each response is 4B for key ID and 2B for key length)
			keylist.clear();
			throw keyListExc;
		}
		// iterate over buffer reading the IDs and key length
		uint16_t offset = 0;
		uint32_t keyid = 0;
		uint16_t keylen = 0;
		while(offset < resp_len){
			memcpy(&keyid, buffer.get()+offset, 4);
			offset+=4;
			memcpy(&keylen, buffer.get()+offset, 2);
			offset+=2;
			if(keyid == 0){
				return; // when the SEcube reaches the end of the flash (all keys returned) it sends 0, so we have our condition to terminate
			}
			std::pair<uint32_t, uint16_t> p(keyid, keylen);
			keylist.push_back(p); // copy ID in list
			keyid = 0;
			keylen = 0;
		}
	}

}

void L1::L1FindKey(uint32_t keyId, bool& found) {
	L1FindKeyException findKeyExc;
	this->base.FillSessionBuffer((uint8_t*)&keyId, L1Response::Offset::DATA, 4);
	uint16_t dataLen = 4;
	uint16_t respLen = 0;
	try {
		TXRXData(L1Commands::Codes::KEY_FIND, dataLen, 0, &respLen);
	}
	catch(L1Exception& e) {
		throw findKeyExc;
	}
	if(respLen != 1){
		throw findKeyExc;
	} else {
		uint8_t res;
		this->base.ReadSessionBuffer((uint8_t*)&res, L1Response::Offset::DATA, 1);
		if(res){
			found = true;
		} else {
			found = false;
		}
	}
}
