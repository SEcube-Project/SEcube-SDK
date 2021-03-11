/**
  ******************************************************************************
  * File Name          : L1.h
  * Description        : Attributes and the methods of the L1 class.
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

/*! \file  L1.h
 *  \brief This header file defines the attributes and the methods of the L1 class.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef L1_H_
#define L1_H_

#include "../L0/L0.h"
#include "L1 Base/L1_base.h"
#include "Login-Logout API/login_logout_api.h"
#include "Security API/security_api.h"
#include "Utility API/utility_api.h"

/** This class defines the attributes and the methods of a L1 object. L1 is built upon L0, therefore it uses a higher
 *  level of abstraction. L0 is focused on very basic actions (such as low level USB communication with the SEcube),
 *  L1 is focused on more "generic" actions, such as the encryption of data or the login to the SEcube using a specific
 *  PIN code. L0 should be used in the initial stage of any SEcube-related software, for instance to list all the available
 *  SEcube devices. Then, after a specific SEcube device has been selected, L1 comes into play allowing to login to the
 *  SEcube and to perform more complex operations (i.e. encrypt data). If a higher level of abstraction is needed (i.e.
 *  to perform even more complex operations, such as working on an encrypted file) then the libraries belonging to the L2
 *  level can be used (although it should be noted that a corresponding L2 object does not exist, since these libraries
 *  offer specific APIs to the developers).  */
class L1 : private L0, public LoginLogoutApi, public SecurityApi, public UtilityApi {
private:
	L1Base base;
	uint8_t index; // this is used only by SEkey to support multiple SEcube connected to the same host computer (default value 255)
	/* these are private methods that are used exclusively for internal and low level purposes. */
	void SessionInit();
	void PrepareSessionBufferForChallenge(uint8_t* cc1, uint8_t* cc2, uint16_t access);
	void TXRXData(uint16_t cmd, uint16_t reqLen, uint16_t cmdFlags, uint16_t* respLen);
	void Se3PayloadCryptoInit();
	void Se3PayloadEncrypt(uint16_t flags, uint8_t* iv, uint8_t* data, uint16_t nBlocks, uint8_t* auth);
	void Se3PayloadDecrypt(uint16_t flags, const uint8_t* iv, uint8_t* data, uint16_t nBlocks, const uint8_t* auth);
	void L1Config(uint16_t type, uint16_t op, std::array<uint8_t, L1Parameters::Size::PIN>& value);
	void KeyList(uint16_t maxKeys, uint16_t skip, se3Key* keyArray, uint16_t* count);
public:
	L1(); /**< Default constructor. */
	L1(uint8_t index); /**< Custom constructor used only in a very specific case by the APIs of the SEkey library (L2). Do not use elsewhere. */
	~L1(); /**< Destructor. Automatic logout implemented. */

	//LOGIN-LOGOUT API
	/** @brief Login to the SEcube.
	 * @param [in] pin The pin to be used (the size of the PIN is always 32 byte).
	 * @param [in] access The privilege level (administrator or user).
	 * @param [in] force True to force logout if previous login still active on the SEcube, false otherwise.
	 * @detail The recommended value for the last parameter is always true. Since there is no return value, exceptions are triggered if
	 * any problem arises (i.e. wrong PIN); exceptions must be managed by the caller. */
	void L1Login(const std::array<uint8_t, L1Parameters::Size::PIN>& pin, se3_access_type access, bool force) override ;
	/** @brief Logout from the SEcube.
	 *  @detail Since there is no return value, exceptions are triggered if any problem arises (i.e. wrong PIN); exceptions must be managed by the caller. */
	void L1Logout() override ;
	/** @brief Same as standard logout, used internally by L1Login if force parameter is true.
	 *  @detail This function is not intended to be used explicitly. */
	void L1LogoutForced();
	/** @brief Returns true if logged in, false otherwise.
	 *  @detail The returned value does not depend on the privilege associated to the login (i.e. administrator or user). */
	bool L1GetSessionLoggedIn();
	/** @brief Returns the privilege level obtained with the login operation. */
	se3_access_type L1GetAccessType();

	//SECURITY API
	/** @brief Initialize the crypto context needed to perform an encryption or decryption operation.
	 * @param [in] algorithm The algorithm to be used (see L1Algorithms::Algorithms).
	 * @param [in] mode A combination of the direction (i.e. encrypt or decrypt) and algorithm mode (i.e. CTR). See CryptoInitialisation::mode and CryptoInitialisation::Feedback.
	 * @param [in] keyId The ID of the key to be used to perform the operation.
	 * @param [out] sessId Here is stored the identifier of the crypto context that is initialized. Must be used later by L1CryptoUpdate().
	 * @detail This is a low level function to exploit the crypto features of the SEcube. It can be ignored, we suggest using L1Encrypt(), L1Decrypt() and L1Digest() instead. */
	void L1CryptoInit(uint16_t algorithm, uint16_t mode, uint32_t keyId, uint32_t& sessId) override ;
	/** @brief Use the crypto context initialized by L1CryptoInit() to perform the corresponding action on a specific portion of data.
	 * @param [in] sessId The id previously set by L1CryptoUpdate().
	 * @param [in] flags Specific flag for this operation, see L1Crypto::UpdateFlags.
	 * @param [in] data1Len The length of the first buffer to be processed by this crypto operation (can be 0).
	 * @param [in] data1 The first buffer to be processed by this crypto operation (can be NULL).
	 * @param [in] data2Len The length of the second buffer to be processed by this crypto operation (can be 0).
	 * @param [in] data2 The second buffer to be processed by this crypto operation (can be NULL).
	 * @param [out] dataOutLen The length of the output of the crypto operation.
	 * @param [in] dataOut The buffer filled with the result of the crypto operation.
	 * @detail This is a low level function to exploit the crypto features of the SEcube. It can be ignored, we suggest using L1Encrypt(), L1Decrypt() and L1Digest() instead. */
	void L1CryptoUpdate(uint32_t sessId, uint16_t flags, uint16_t data1Len, uint8_t* data1, uint16_t data2Len, uint8_t* data2, uint16_t* dataOutLen, uint8_t* dataOut) override ;
	/** @brief Encrypt some data according to a specific algorithm and mode (i.e. AES-256-CBC), using a specific key.
	 * @param [in] plaintext_size The length of the buffer to be encrypted.
	 * @param [in] plaintext The buffer to be encrypted.
	 * @param [out] encrypted_data The L1Ciphertext object where the encrypted data and other metadata will be stored.
	 * @param [in] algorithm The algorithm to be used (see L1Algorithms::Algorithms).
	 * @param [in] algorithm_mode The mode of the algorithm (i.e. CBC, CTR, etc.). See CryptoInitialisation::Feedback.
	 * @param [in] key_id The ID of the key to be used to perform the operation.
	 * @detail Throws exception in case of errors. */
	void L1Encrypt(size_t plaintext_size, std::shared_ptr<uint8_t[]> plaintext, SEcube_ciphertext& encrypted_data, uint16_t algorithm, uint16_t algorithm_mode, uint32_t key_id) override ;
	/** @brief Decrypt data that were previously encrypted using L1Encrypt().
	 * @param [in] encrypted_data The L1Ciphertext object where the encrypted data and other metadata is stored.
	 * @param [out] plaintext_size The size (bytes) of the decrypted data.
	 * @param [out] plaintext The buffer holding the decrypted data.
	 * @detail Throws exception in case of errors. The L1Ciphertext object passed as parameter should be configured with all the details required by the decryption process.
	 * If the object was generated calling the L1Encrypt() API, then it is already ok. If the data to decrypt have not been encrypted with the L1Encrypt() function, they
	 * must be encapsulated into a L1Ciphertext object; then the object must be configured with the required parameters (i.e. algorithm, mode, nonce, iv, etc...) so that
	 * the L1Decrypt() can perform its task. */
	void L1Decrypt(SEcube_ciphertext& encrypted_data, size_t& plaintext_size, std::shared_ptr<uint8_t[]>& plaintext) override ;
	/** @brief Compute the digest of some data.
	 * @param [in] input_size The length of the buffer to be processed.
	 * @param [in] input_data The buffer to be processed.
	 * @param [out] digest The object where the digest will be stored.
	 * @detail Before calling this function, you must setup some of the parameters of the digest object that you are going to pass.
	 * Check the documentation of the digest object. For instance, if you want to generate the SHA-256 digest you simply need to set
	 * the "algorithm" parameter. If you want to use the HMAC-SHA256, you also need to set other parameters. */
	void L1Digest(size_t input_size, std::shared_ptr<uint8_t[]> input_data, SEcube_digest& digest) override ;
	/* @brief Set the PIN for administrator privilege level on the SEcube.
	 * @param [in] pin The PIN to be set.
	 * @detail Throws exception in case of errors. */
	void L1SetAdminPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) override ;
	/* @brief Set the PIN for user privilege level on the SEcube.
	 * @param [in] pin The PIN to be set.
	 * @detail Throws exception in case of errors. */
	void L1SetUserPIN(std::array<uint8_t, L1Parameters::Size::PIN>& pin) override ;
	/* @brief Function to manually add or remove keys in the SEcube device. Use this for test purposes or if you do not want to use SEkey.
	 * @param [in] k The key to be written to the SEcube or removed.
	 * @param [in] op The type of operation. See L1Commands::KeyOpEdit for info.
	 * @detail Throws exception in case of errors. When op = L1Commands::KeyOpEdit::SE3_KEY_OP_ADD the parameter k must be filled with all
	 * attributes (id, key size, key value). When op = L1Commands::KeyOpEdit::SE3_KEY_OP_DELETE the parameter k can be filled with the ID
	 * of the key to be deleted, key size must be set to 0 and key value must be NULL. When op = L1Commands::KeyOpEdit::SE3_KEY_OP_ADD_TRNG
	 * the parameter k must be filled with the ID of the key to be added and with the desired key size, key value must be NULL because the
	 * value of the key will be computed inside the SEcube with the TRNG. Notice that the key ID of the keys managed with this API must be
	 * inside the range reserved to keys that are manually managed and out of the scope of SEkey. */
	void L1KeyEdit(se3Key& k, uint16_t op) override ;
	/* @brief List the keys stored inside the memory of a SEcube device.
	 * @param [out] keylist The list of keys inside the SEcube (ID, length).
	 * @detail This function is dedicated to manual key management, therefore only the keys that are not managed by SEkey will be listed. Throws exception in case of errors. */
	void L1KeyList(std::vector<std::pair<uint32_t, uint16_t>>& keylist) override ;
	/* @brief Check if the key with the specified ID is stored inside the SEcube.
	 * @param [in] key_id The ID of the key to search.
	 * @param [out] found Boolean that stores the result of the search. True if the key is found, false otherwise.
	 * @detail Throws exception in case of errors. There is no limitation in terms of IDs that can be passed (everything in range from 0 to 2^32-1 is fine). */
	void L1FindKey(uint32_t key_id, bool& found) override ;
	/** @brief Retrieve the list of algorithms supported by the device.
	 * @param [out] algorithmsArray */
	void L1GetAlgorithms(std::vector<se3Algo>& algorithmsArray) override ;

	// Other API
	/** @brief Select a specific SEcube out of multiple SEcube devices.
	 * @param [in] sn The serial number of the SEcube to be selected.
	 * @detail The selected SEcube will be the one used by the host PC to perform required actions (i.e. data encryption). Throws exception in case of errors. */
	void L1SelectSEcube(std::array<uint8_t, L0Communication::Size::SERIAL>& sn);
	/** @brief Select a specific SEcube out of multiple SEcube devices.
	 * @param [in] indx The number of the SEcube to be selected.
	 * @details The selected SEcube will be the one used by the host PC to perform required actions (i.e. data encryption). The parameter is used to identify the SEcube in the
	 * list of SEcube devices that is returned by L0 GetDeviceList(). Throws exception in case of errors. */
	void L1SelectSEcube(uint8_t indx);
	/** @brief Initialize the SEcube with the specified serial number (this is a wrapper of a similar L0 factory init function).
	 * @param [in] serialno The serial number to be set on the SEcube.
	 * @details Throws exception if the SEcube is already initialized (or in case of any error). Since it simply is a wrapper around the corresponding L0 function, this does not require to be logged in to the SEcube. */
	void L1FactoryInit(const std::array<uint8_t, L0Communication::Size::SERIAL>& serialno);
	/** @brief Get the serial number of the SEcube.
	 * @param [out] sn The string where the serial number will be stored. */
	void GetDeviceSerialNumber(std::string& sn);

	// L1 API implemented to support SEkey API (should not be used explicitly)
	/** @brief Read or write the user ID and the user name of the SEcube owner (member of SEkey) from/to the SEcube. Used only by SEkey, do not use explicitly.
	 * @param [in] id The string where the user ID is stored.
	 * @param [in] name The string where the user name is stored.
	 * @param [in] mode Read or write, according to L1SEkey::Direction.
	 * @return True on success, false otherwise. If mode is read, the user ID and the user name are returned through the respective parameters. If mode is write, those values are stored into the SEcube. */
	bool L1SEkey_Info(std::string& id, std::string& name, uint8_t mode);
	/** @brief Export a key from the SEcube flash memory, wrapping it with another key. Used only by SEkey, do not use explicitly.
	 * @param [in] key_export_id The ID of the key to be exported.
	 * @param [in] key_wrapping_key The ID of the wrapping key.
	 * @param [out] key_export_data The byte array where the wrapped key will be stored.
	 * @param [out] key_export_len The length of the exported key (length of the ciphertext).
	 * @return True on success, false otherwise. */
	bool L1SEkey_GetKeyEnc(uint32_t key_export_id, uint32_t key_wrapping_key, std::shared_ptr<uint8_t[]>& key_export_data, uint16_t& key_export_len);
	/** @brief This is used to list the IDs of all the keys stored inside a SEcube device. Used only by SEkey, do not use explicitly.
	 * @param [in] buffer The buffer where the result is stored (6010 bytes recommended size).
	 * @param [out] buflen The length of the result.
	 * @details This function is used exclusively by SEkey to perform a sort of garbage collector, it should not be used in any other case. */
	void L1SEkey_Maintenance(uint8_t *buffer, uint16_t *buflen);
	/** @brief Delete the key with the specified ID from the SEcube. Used only by SEkey, do not use explicitly.
	 * @param [in] key_id The ID of the key to be deleted.
	 * @return True on success, false otherwise. */
	bool L1SEkey_DeleteKey(uint32_t key_id);
	/** @brief Check if the SEcube is ready for SEkey (meaning that it has been initialized with the correct keys).
	 * @return True if the SEcube is initialized for SEkey, false otherwise. */
	bool L1SEkey_isReady();
	/** @brief Delete all the keys from the SEcube, except for the keys specified in the keep parameter. Used only by SEkey, do not use explicitly.
	 * @param [in] keep The IDs of the keys that must not be deleted.
	 * @return True on success, false otherwise. */
	bool L1SEkey_DeleteAllKeys(std::vector<uint32_t>& keep);
	/** @brief Write a key into the SEcube. The key to be written may still be wrapped with another key. Used only by SEkey, do not use explicitly.
	 * @param [in] key_id The ID of the key to be written.
	 * @param [in] key_len The length of the key to be written.
	 * @param [in] dec_id The ID of the key to be used to unwrapped the key that needs to be written.
	 * @param [in] key_data The value of the key to be written. Can be null if the key must be generated inside the SEcube with the TRNG.
	 * @return True on success, false otherwise. */
	bool L1SEkey_InsertKey(uint32_t key_id, uint16_t key_len, uint32_t dec_id, std::shared_ptr<uint8_t[]> key_data);
};

#endif
