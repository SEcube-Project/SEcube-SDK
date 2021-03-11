/**
  ******************************************************************************
  * File Name          : encryption_algorithms.cpp
  * Description        : Usage example of L1 encryption APIs.
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

/*! \file  encryption_algorithms.cpp
 *  \brief This file is an example about how to encrypt and decrypt data with a SEcube device.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include "../sources/L1/L1.h"
#include <thread> // thread::sleep_for

using namespace std;

#define TESTSIZE 1024 // change this according to the size of the data you want to encrypt in this example

// RENAME THIS TO main()
int encrypt_example() {

	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	cout << "Welcome to SEcube data encryption / decryption example!" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000)); // these delays are simply here to slow down the process making it more comprehensible
	cout << "Looking for SEcube devices...\n" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));

	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if(numdevices == 0){
		cout << "No SEcube devices found! Quit." << endl;
		return 0;
	}
	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if(ret){
		cout << "Error while searching for SEcube devices! Quit." << endl;
		return -1;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	cout << "List of SEcube devices (path, serial number):" << endl;
	int index = 0;
	uint8_t empty_serial_number[L0Communication::Size::SERIAL];
	memset(empty_serial_number, 0, L0Communication::Size::SERIAL);
	for(pair<string, string> p : devices){
		if(p.second.empty() || memcmp(p.second.data(), empty_serial_number, L0Communication::Size::SERIAL)==0){
			cout << index << ") " << p.first << " - serial number not available (please initialize this SEcube)" << endl;
		} else {
			cout << index << ") " << p.first << " - " << p.second << endl;
		}
		index++;
	}
	int sel = 0;
	cout << "\nEnter the number corresponding to the SEcube device that you want to use..." << endl;
	/* warning: if cin does not wait for input in debug mode with eclipse, open the launch configuration and select
	 * the "use external console for inferior" checkbox under the debugger tab (see https://stackoverflow.com/questions/44283534/c-debug-mode-in-eclipse-causes-program-to-not-wait-for-cin)*/
	if(!(cin >> sel)){
		cout << "Input error...quit." << endl;
		return -1;
	}

	if((sel >= 0) && (sel < numdevices)){
		array<uint8_t, L0Communication::Size::SERIAL> sn;
		sn.fill(0);
		if(devices.at(sel).second.length() > L0Communication::Size::SERIAL){
			cout << "Unexpected error...quit." << endl;
			return -1;
		} else {
			memcpy(sn.data(), devices.at(sel).second.data(), devices.at(sel).second.length());
		}
		l1->L1SelectSEcube(sn); // select secube with correct serial number
		cout << "\nDevice " << devices.at(sel).first << " - " << devices.at(sel).second << " selected." << endl;

		array<uint8_t, 32> pin = {'t','e','s','t'}; // customize this PIN according to the PIN that you set on your SEcube device
		l1->L1Login(pin, SE3_ACCESS_USER, true); // login to the SEcube
		cout << "SEcube login OK." << endl;

		cout << "\n\nThe SEcube SDK currently supports the following algorithms:" << endl;
		vector<se3Algo> algos;
		try{
			l1->L1GetAlgorithms(algos);
		} catch (...) {
			cout << "Unexpected error...quit." << endl;
			l1->L1Logout();
			return -1;
		}
		for(se3Algo algo : algos){
			algo.print();
		}

		cout << "\nWe are going to encrypt a buffer containing random data, then we will decrypt it and check if the decrypted data match with the original data." << endl;
		shared_ptr<uint8_t[]> plaintext(new uint8_t[TESTSIZE]); // create a byte array where the plaintext is stored
		L0Support::Se3Rand(TESTSIZE, plaintext.get()); // fill the array with random data

		/* Here we give to the user the possibility to choose the algorithm and the mode for encryption. This is not mandatory, you can write your code so that it always uses a specific
		 * algorithm and a specific mode. For example AES-256-CTR-HMAC-SHA-256. A correct implementation always uses the best algorithm depending on the context, not depending on the choice
		 * of the user. Notice that here we hard-code a menu for the selection, however you can always retrieve which algorithms are available with L1GetAlgorithms(). */
		cout << "\nPlease enter the number associated to the algorithm and mode for encryption:" << endl;
		cout << "0) AES-ECB" << endl;
		cout << "1) AES-CBC" << endl;
		cout << "2) AES-CTR" << endl;
		cout << "3) AES-CFB" << endl;
		cout << "4) AES-OFB" << endl;
		cout << "5) AES-EBC + HMAC-SHA-256" << endl;
		cout << "6) AES-CBC + HMAC-SHA-256" << endl;
		cout << "7) AES-CTR + HMAC-SHA-256" << endl;
		cout << "8) AES-CFB + HMAC-SHA-256" << endl;
		cout << "9) AES-OFB + HMAC-SHA-256" << endl;
		sel = 0;
		if(!(cin >> sel)){
			cout << "Input error...quit." << endl;
			l1->L1Logout();
			return -1;
		}

		cout << "\nThese are the keys stored on the SEcube." << endl;
		cout << "Please enter the number of the key that you would like to use for encryption." << endl;
		cout << "Notice that, if you choose a key that is not compatible with the algorithm you chose at the"
				"previous step, encryption will fail." << endl;
		vector<pair<uint32_t, uint16_t>> keys;
		try{
			l1->L1KeyList(keys);
		} catch (...) {
			cout << "Error retrieving keys inside the SEcube device. Quit." << endl;
			l1->L1Logout();
			return -1;
		}
		if(keys.size() == 0){
			cout << "There are no keys inside the SEcube device. Impossible to continue." << endl;
			l1->L1Logout();
			return -1;
		}
		int cnt = 0;
		for(pair<uint32_t, uint16_t> k : keys){
			cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;
			cnt++;
		}
		uint32_t key = 0;
		int ch = 0;
		if(!(cin >> ch)){
			cout << "Input error...quit." << endl;
			l1->L1Logout();
			return -1;
		}


		cout << "\nStarting encryption..." << endl;
		SEcube_ciphertext encrypted_data;
		key = keys.at(ch).first;
		/* Notice how we use the L1Encrypt() API depending on the algorithm and mode selected by the user. The L1Ciphertext holds the encrypted data and other
		 * metadata (such as the initialization vector to perform the decryption). We use the key ID 1 just for sake of simplicity, you can use any other suitable
		 * key stored inside your SEcube. */
		switch(sel){
			case 0:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::ECB, key);
				break;
			case 1:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::CBC, key);
				break;
			case 2:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::CTR, key);
				break;
			case 3:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::CFB, key);
				break;
			case 4:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES, CryptoInitialisation::Modes::OFB, key);
				break;
			case 5:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES_HMACSHA256, CryptoInitialisation::Modes::ECB, key);
				break;
			case 6:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES_HMACSHA256, CryptoInitialisation::Modes::CBC, key);
				break;
			case 7:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES_HMACSHA256, CryptoInitialisation::Modes::CTR, key);
				break;
			case 8:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES_HMACSHA256, CryptoInitialisation::Modes::CFB, key);
				break;
			case 9:
				l1->L1Encrypt(TESTSIZE, plaintext, encrypted_data, L1Algorithms::Algorithms::AES_HMACSHA256, CryptoInitialisation::Modes::OFB, key);
				break;
			default:
				cout << "Input error...quit." << endl;
				l1->L1Logout();
				return -1;
		}

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "Here are the first 30 bytes of the plaintext (or less if the plaintext is shorter) in hex format:" << endl;
		for(unsigned int i=0; i<30; i++){
			if(i<TESTSIZE){
				printf("%02x ", plaintext[i]);
			}
		}

		cout << "\n\nHere are the first 30 bytes of the ciphertext (or less if the ciphertext is shorter) in hex format." << endl;
		cout << "Notice how the hex values are different, meaning that the plaintext has been changed into a non-intelligible form." << endl;
		for(unsigned int i=0; i<30; i++){
			if(i<encrypted_data.ciphertext_size){
				printf("%02x ", encrypted_data.ciphertext[i]);
			}
		}

		cout << "\n\nStarting decryption..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));

		shared_ptr<uint8_t[]> decrypted_data;
		size_t decrypted_data_size;
		/* Notice that we call the L1Decrypt API passing the L1Ciphertext object. The L1Decrypt does not simply decrypt the data, if AES-HMAC-SHA-256 was used
		 * for encryption then it also checks the signature for integrity and authentication of data. */
		l1->L1Decrypt(encrypted_data, decrypted_data_size, decrypted_data);

		cout << "\n\nHere are the first 30 bytes of the decrypted data (or less if the plaintext is shorter) in hex format:" << endl;
		for(unsigned int i=0; i<30; i++){
			if(i<decrypted_data_size){
				printf("%02x ", decrypted_data[i]);
			}
		}
		cout << endl;

		/* For the sake of this example, we directly compare the original data with the decrypted data. This is not necessary at all, because
		 * the L1Decrypt already checks the signature if AES-HMAC-SHA-256 is used (no signature in simple AES mode). */
		if(memcmp(plaintext.get(), decrypted_data.get(), TESTSIZE) == 0 && (decrypted_data_size == TESTSIZE)){
			cout << "\nThe data before encryption and after decryption are the same. Success!" << endl;
		} else {
			cout << "\nThe data before encryption and after decryption are not the same. Fail!" << endl;
		}

		l1->L1Logout();
		cout << "\nExample completed. Press 'q' to quit." << endl;
		while(cin.get() != 'q'){};
	} else {
		cout << "You entered an invalid number. Quit." << endl;
		return 0;
	}
	return 0;
}
