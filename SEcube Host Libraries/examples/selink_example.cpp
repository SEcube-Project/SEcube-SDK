/**
  ******************************************************************************
  * File Name          : selink_example.cpp
  * Description        : Usage example of SElink APIs.
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

/*! \file  selink_example.cpp
 *  \brief This is an example to show you how the APIs of SElink can be used.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include <fstream>
#include "../sekey/SEkey.h"
#include "../selink/SElink.h"
#include "../sefile/SEcureDB.h"
#include <thread> // thread::sleep_for

using namespace std;

/* This example does not include the usage of SEkey, since the specific
 * SElink API compatible with SEkey behaves differently depending on the
 * KMS configuration (i.e. we don't know which users and groups are present
 * in your KMS). Notice that the usage of selink_encrypt_auto() instead of
 * selink_encrypt_manual() is straightforward since you only need to provide
 * the list of recipients of the message. */

// RENAME THIS TO main()
int selink_example() {
	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	array<uint8_t, L1Parameters::Size::PIN> pin = {'t','e','s','t'};

	cout << "--- SElink library example ---" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000)); // these delays are simply here to slow down the process making it more comprehensible
	cout << "Looking for SEcube devices..." << endl;
	this_thread::sleep_for(chrono::milliseconds(1000));

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if(ret){
		cout << "\nError while searching for SEcube devices! Quit." << endl;
		return -1;
	}
	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if(numdevices == 0){
		cout << "\nNo SEcube devices found! Quit." << endl;
		return 0;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	cout << "List of SEcube devices (path - serial number):" << endl;
	int index = 0;
	for(pair<string, string> p : devices){
		cout << index << ") " << p.first << " - " << p.second << endl;
		index++;
	}

	int sel = 0;
	cout << "\nEnter the number corresponding to the SEcube device you want to use..." << endl;
	/* warning: if cin does not wait for input in debug mode with eclipse, open the launch configuration and select
	 * the "use external console for inferior" checkbox under the debugger tab (see https://stackoverflow.com/questions/44283534/c-debug-mode-in-eclipse-causes-program-to-not-wait-for-cin)*/
	if(!(cin >> sel)){
		cout << "\nInput error...quit." << endl;
		return -1;
	}

	if((sel >= 0) && (sel < numdevices)){
		std::array<uint8_t, L0Communication::Size::SERIAL> sn;
		sn.fill(0);
		if(devices.at(sel).second.length() > L0Communication::Size::SERIAL){
			cout << "\nUnexpected error...quit." << endl;
			return -1;
		} else {
			memcpy(sn.data(), devices.at(sel).second.data(), devices.at(sel).second.length());
		}
		l1->L1SelectSEcube(sn);
		cout << "Selected device " << devices.at(sel).first << " - " << devices.at(sel).second << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		try{
			l1->L1Login(pin, SE3_ACCESS_USER, true); // provide pin, access privilege and boolean to force logout before login (in case there was already an active login on the device)
			/*
			 * notice that when the login is executed, the SEcube device enters a specific loop to handle requests coming from the host.
			 * if you close your program without performing the logout, the SEcube device will still be in that loop the next time you
			 * will attempt the login (provided that you do not unplug the SEcube from the PC, removing power). this is the reason why
			 * you have to provide the boolean as third parameter of the L1Login function, because you need to force the logout from
			 * previous pending sessions. if you do not force the logout, you won't be able to communicate with the SEcube (because the
			 * session is still open on the SEcube side, but it doesn't exist anymore on your host).
			 */
		} catch (...) { // catch any kind of exception (login will throw if the password is wrong or if any error happens)
			cout << "\nSEcube login error. Quit." << endl;
			return 0;
		}
		if(!l1->L1GetSessionLoggedIn()){ // check if login was ok
			cout << "\nSEcube login error. Quit." << endl;
			return 0;
		} else {
			cout << "SEcube login OK" << endl;
		}


		/* Begin of SElink example */

		cout << "\nThese are the keys stored on the SEcube." << endl;
		cout << "Please enter the number of the key that you would like to use." << endl;
		vector<pair<uint32_t, uint16_t>> keys;
		try{
			l1->L1KeyList(keys);
		} catch (...) {
			cout << "\nError retrieving keys inside the SEcube device. Quit." << endl;
			l1->L1Logout();
			return -1;
		}
		if(keys.size() == 0){
			cout << "\nThere are no keys inside the SEcube device. Impossible to continue." << endl;
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
			cout << "\nInput error...quit." << endl;
			l1->L1Logout();
			return -1;
		}
		key = keys.at(ch).first;

		cout << "\nReading the content of the file named 'sample_file_input.selink'..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		ifstream testfile("sample_file_input.selink", ios::binary);
		if(!testfile.is_open()){
			cout << "\nError reading 'sample_file_input.selink'. Quit." << endl;
			l1->L1Logout();
			return -1;
		}
		std::vector<unsigned char> plaintext(istreambuf_iterator<char>(testfile), {});
		testfile.close();
		size_t plaintext_size = plaintext.size();
		cout << "File 'sample_file_input.selink' correctly read." << endl;
		cout << "The content of 'sample_file_input.selink' is now stored in RAM memory, content size is " << plaintext_size << " bytes." << endl;

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\nStarting to encrypt the content of the file, using the SElink library..." << endl;
		SElink ciphertext;
		shared_ptr<uint8_t[]> plaintext_ptr(new uint8_t[plaintext_size]);
		memcpy(plaintext_ptr.get(), plaintext.data(), plaintext_size);
		int rc = selink_encrypt_manual(l1.get(), plaintext_ptr, plaintext_size, ciphertext, key); // encrypt the plaintext using SElink manual API (the API for the KMS works in the same way)
		if(rc != SELINK_OK){
			cout << "\nError encrypting data. Quit.";
			l1->L1Logout();
			return -1;
		}
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "File content encrypted correctly. Debug info:" << endl;
		ciphertext.printdebug();

		// serialize the encrypted data
		cout << "\nStarting to serialize the encrypted data. After serialization, they can be sent to the recipient.\n"
				"In our case, we will simply store the serialized data in a file on disk, the file will be read and the\n"
				"data will be deserialized in order to be decrypted and compared with the original file." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		size_t serialized_size = 0;
		unique_ptr<char[]> serialized;
		ciphertext.serialize(serialized, serialized_size);
		// write serialized data to file (this is not necessary, you can do whatever you want with your encrypted data, i.e. sending them over a TCP connection)
		ofstream ff("serialized.selink", ios::binary);
		ff.write((const char*)serialized.get(), serialized_size);
		ff.close();
		cout << "\nData serialization done. Serialized data written to file." << endl;

		// read serialized data from file
		cout << "Reading file with serialized data..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		ifstream testfile2("serialized.selink", ios::binary);
		std::vector<char> serializedfile(istreambuf_iterator<char>(testfile2), {});
		testfile2.close();
		serialized_size = serializedfile.size();

		// deserialize the data, placing the deserialized buffer (which is the ciphertext) into a ciphertext object
		cout << "Deserializing the data..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		unique_ptr<char[]> deserialized_ptr = make_unique<char[]>(serialized_size); // first we must copy the data from the vector to a unique_ptr
		memcpy(deserialized_ptr.get(), serializedfile.data(), serialized_size);
		SElink deserialized; // this obejct holds the ciphertext
		deserialized.deserialize(deserialized_ptr, serialized_size);

		// decrypt the ciphertext
		cout << "Decrypting the data..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		size_t decrypted_size;
		shared_ptr<uint8_t[]> decrypted;
		rc = selink_decrypt(l1.get(), decrypted, decrypted_size, deserialized);
		if(rc != SELINK_OK){
			cout << "\nError decrypting data. Quit.";
			l1->L1Logout();
			return -1;
		}
		cout << "Length of decrypted data: " << decrypted_size << endl;

		// notice that this check is not necessary because SElink automatically returns an error if the signatures of the data do not match (so if they match we know the plaintexts are equal)
		cout << "\n/*** CHECKING IF ORIGINAL DATA AND DECRYPTED DATA ARE EQUAL ***/" << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		bool equal = true;
		if(plaintext_size != decrypted_size){
			equal = false;
		} else {
			for(size_t i=0; i<plaintext_size; i++){
				if((plaintext[i] != decrypted[i])){
					equal = false;
					break;
				}
			}
		}
		if(equal){
			cout << "Data are the same!" << endl;
		} else {
			cout << "Data are not the same!" << endl;
		}
		cout << "\nNotice that this check is never required by SElink.\nThe library, when decrypting some data, "
				"takes as input a SElink object that includes a ciphertext along with a signature.\nAfter decrypting"
				" the ciphertext, the signature is recomputed on the decrypted data and the two signatures are compared.\n"
				"If they are the same, then the decryption process was correct and the data are intact, meaning that both "
				"integrity and authentication of the sender are granted." << endl;

		// write decrypted data to a file so that it can be compared with the original one
		ofstream ff2("sample_file_output.selink", ios::binary);
		ff2.write((const char*)decrypted.get(), decrypted_size);
		ff2.close();
		cout << "\nA copy of the decrypted data has been written into 'sample_file_output.selink'.\n"
				"You can manually check that the two files are equal (i.e. diff command on Linux)." << endl;

		/* SElink example end */

		try{
			l1->L1Logout();
		} catch (...) {
			cout << "Logout error. Quit." << endl;
			return -1;
		}
		if(l1->L1GetSessionLoggedIn()){ // check if logout was ok
			cout << "Logout error. Quit." << endl;
			return -1;
		}
		cout << "\nSElink example completed. Press q to quit." << endl;
		while(cin.get() != 'q'){};
	} else {
		cout << "You entered an invalid number. Quit." << endl;
		return -1;
	}

	return 0;
}
