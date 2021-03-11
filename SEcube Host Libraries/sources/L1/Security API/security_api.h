/**
  ******************************************************************************
  * File Name          : security_api.h
  * Description        : Functions and data structures related to the security domain of L1 APIs.
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

/*! \file  security_api.h
 *  \brief This header file defines functions and data structures related to the security domain of L1 APIs.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef SECURITY_API_H_
#define SECURITY_API_H_

#include "../L1 Base/L1_base.h"

/** This class is used to store the result of the L1Digest() API. It is required, in particular, when the algorithm used to
 *  generate the digest is HMAC-SHA256 because this algorithm requires the usage of a shared secret (a key) and a nonce (to avoid
 *  replay), hence the attributes of this class. If you simply want to compute the digest with SHA-256, then the nonce and
 *  the key are not used at all, therefore the only attribute you care about is the digest. */
class SEcube_digest{
public:
	uint32_t key_id; /**< The ID of the key that is used to generate the digest with HMAC-SHA256. */
	uint16_t algorithm; /**< The algorithm used to generate the digest. */
	std::array<uint8_t, B5_SHA256_DIGEST_SIZE> digest; /**< The digest of the data. The size is B5_SHA256_DIGEST_SIZE because current digest algorithms always produce a result on 32 bytes. */
	std::array<uint8_t, B5_SHA256_DIGEST_SIZE> digest_nonce; /**< This is the nonce that is used to compute the authenticated digest with HMAC-SHA256. */
	bool usenonce; /**< Use the nonce parameter as input to generate the digest. This can be useful if you already have a digest that was computed on the data
	you are working on, and you want to recompute it to check if the data have been modified. So you also need to set the same nonce that was used in the previous
	computation. */
};

/** This class implements a L1Ciphertext object, which is used exclusively by L1Encrypt() and L1Decrypt().
 *  For these APIs, a dedicated object is required because it is important to keep track of every detail of
 *  the crypto computation, for example which nonce or initialization vector was used. The user should not
 *  care about these details, but they are required to correctly perform different operations, for example
 *  to decrypt some data with L1Decrypt() after having encrypted them with L1Encrypt(). */
class SEcube_ciphertext{
public:
	uint32_t key_id; /**< The ID of the key that was used to perform the crypto operation. */
	uint16_t algorithm; /**< The algorithm used to perform the crypto operation (i.e. AES). */
	uint16_t mode; /**< The mode of the algorithm (i.e. CTR). */
	std::unique_ptr<uint8_t[]> ciphertext; /**< The buffer holding the encrypted data. */
	size_t ciphertext_size; /**< The dimension of the ciphertext (bytes). */
	std::array<uint8_t, B5_SHA256_DIGEST_SIZE> digest; /**< The digest that is associated to the data if using AES with HMAC-SHA-256. */
	std::array<uint8_t, B5_SHA256_DIGEST_SIZE> digest_nonce; /**< This is the nonce that is used to compute the authenticated digest. */
	std::array<uint8_t, B5_AES_BLK_SIZE> CTR_nonce; /**< This is the nonce that is used to run the AES cipher in CTR mode. */
	std::array<uint8_t, B5_AES_BLK_SIZE> initialization_vector; /**< This is the initialization vector that is used to run AES in CBC, CFB, OFB modes. */
	void reset(); /**< Reset the content of the L1Ciphertext object. */
};

class SecurityApi {
private:
public:
	virtual ~SecurityApi() {};
	virtual void L1KeyList(std::vector<std::pair<uint32_t, uint16_t>>& keylist) = 0;
	virtual void L1KeyEdit(se3Key& k, uint16_t op) = 0;
	virtual void L1FindKey(uint32_t key_id, bool& found) = 0;
	virtual void L1CryptoInit(uint16_t algorithm, uint16_t mode, uint32_t keyId, uint32_t& sessId) = 0;
	virtual void L1CryptoUpdate(uint32_t sessId, uint16_t flags, uint16_t data1Len, uint8_t* data1, uint16_t data2Len, uint8_t* data2, uint16_t* dataOutLen, uint8_t* dataOut) = 0;
	virtual void L1Encrypt(size_t plaintext_size, std::shared_ptr<uint8_t[]> plaintext, SEcube_ciphertext& encrypted_data, uint16_t algorithm, uint16_t algorithm_mode, uint32_t key_id) = 0;
	virtual void L1Decrypt(SEcube_ciphertext& encrypted_data, size_t& plaintext_size, std::shared_ptr<uint8_t[]>& plaintext) = 0;
	virtual void L1Digest(size_t input_size, std::shared_ptr<uint8_t[]> input_data, SEcube_digest& digest) = 0;
	virtual void L1GetAlgorithms(std::vector<se3Algo>& algorithmsArray) = 0;
};

#endif
