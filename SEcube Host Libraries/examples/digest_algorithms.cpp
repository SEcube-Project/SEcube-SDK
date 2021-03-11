/**
  ******************************************************************************
  * File Name          : digest_algorithms.cpp
  * Description        : Usage example of L1 digest APIs.
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

/*! \file  digest_computation.cpp
 *  \brief This is an example about how to compute the digest of some data with the SEcube device.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include "../sources/L1/L1.h"
#include <thread> // sleep_for

using namespace std;

// RENAME THIS TO main()
int digest_example() {

	/* we will compute the digest of this string...you can take this string and use it as input of other tools that compute the
	 * SHA-256 digest (i.e. online tools), so you can check that the results are the same. you can't do the same thing with
	 * HMAC-SHA-256 because it involves the usage of a key (that you can select) stored inside the SEcube device, moreover it also
	 * required a nonce (a random number used only once) that determines also the result. in order to check that HMAC-SHA-256 works
	 * correctly, we will recompute its digest using the same key and the same nonce. */
	char digest_input[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin placerat ornare nunc vitae placerat. "
			"Phasellus tincidunt dictum dui at tristique. Fusce at neque ac nisl hendrerit porta in quis ipsum. In at sollicitudin "
			"nunc, et feugiat augue placerat.";
	int testsize = strlen(digest_input);

	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	cout << "Welcome to SEcube digest example!" << endl;
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

	// for later usage
	vector<pair<uint32_t, uint16_t>> keys;
	int cnt = 0, ch = 0;

	if((sel >= 0) && (sel < numdevices)){
		array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
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

		cout << "\nWe are going to compute the digest of the following string:" << endl;
		cout << digest_input << endl;
		shared_ptr<uint8_t[]> input_data(new uint8_t[testsize]); // to be sent to digest API
		memcpy(input_data.get(), digest_input, testsize);

		/* Here we give to the user the possibility to choose the algoritm and the mode for encryption. This is not mandatory, you can write your code so that it always uses a specific
		 * algorithm and a specific mode. For example AES-256-CTR-HMAC-SHA-256. A correct implementation always uses the best algorithm depending on the context, not depending on the choice
		 * of the user. */
		sel = 0;
		cout << "\nPlease enter the number associated to the algorithm for the digest computation:" << endl;
		cout << "0) SHA-256" << endl;
		cout << "1) HMAC-SHA-256" << endl;
		if(!(cin >> sel)){
			cout << "Input error...quit." << endl;
			l1->L1Logout();
			return -1;
		}

		cout << "\nStarting digest computation..." << endl;
		/* Notice how the L1Digest API is used, passing a Digest object that has been configured with
		 * the right attributes depending on how we want to compute the digest. */
		SEcube_digest data_digest;
		SEcube_digest temp; // this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
		switch(sel){
			case 0:
				// when using SHA-256, you don't need to set anything else than the algorithm
				data_digest.algorithm = L1Algorithms::Algorithms::SHA256;
				l1->L1Digest(testsize, input_data, data_digest);
				break;
			case 1:
				/* when using HMAC-SHA-256, we also need to provide other details. this type of digest is
				 * authenticated by means of a shared secret (i.e. a symmetric key), therefore we must provide
				 * the ID of the key to be used for authentication. we also need to set the value of the usenonce
				 * flag to false or true. this value should always be false, unless you want to compute the digest
				 * using a specific nonce to begin with, which is useful for instance if you already have the value
				 * of the digest computed on the same data with the same algorithm, and you want to recompute it
				 * (therefore using the same nonce you used before) to see if the digest is still the same or not. */
				cout << "\nThese are the keys stored on the SEcube." << endl;
				cout << "Please enter the number of the key that you would like to use for computing the authenticated digest." << endl;
				cout << "Notice that, if you choose a key that is not compatible with the algorithm you chose at the previous step, the digest computation will fail." << endl;
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
				for(pair<uint32_t, uint16_t> k : keys){
					cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;
					cnt++;
				}
				if(!(cin >> ch)){
					cout << "Input error...quit." << endl;
					l1->L1Logout();
					return -1;
				}
				data_digest.key_id = keys.at(ch).first; // use the selected key ID
				data_digest.usenonce = false; // we don't want to provide a specific nonce manually
				data_digest.algorithm = L1Algorithms::Algorithms::HMACSHA256;
				l1->L1Digest(testsize, input_data, data_digest);
				// this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
				temp.key_id = keys.at(ch).first;
				temp.usenonce = true;
				temp.algorithm = L1Algorithms::Algorithms::HMACSHA256;
				temp.digest_nonce = data_digest.digest_nonce;
				l1->L1Digest(testsize, input_data, temp);
				break;
			// notice that, after calling the L1Digest() function, our digest will be stored inside the Digest object
			default:
				cout << "Input error...quit." << endl;
				l1->L1Logout();
				return -1;
		}

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\n\nThe hex value of the digest is:" << endl;
		for(uint8_t i : data_digest.digest){
			printf("%02x ", i);
		}

		// print also recomputed digest (if any)
		if(data_digest.algorithm == L1Algorithms::Algorithms::HMACSHA256){
			cout << "\n\nThe hex value of the recomputed digest is:" << endl;
			for(uint8_t i : temp.digest){
				printf("%02x ", i);
			}
			if(temp.digest == data_digest.digest){
				cout << "\nOriginal digest and recomputed digest are equal. OK." << endl;
			} else {
				cout << "\nOriginal digest and recomputed digest are not equal. Something went wrong..." << endl;
			}
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
