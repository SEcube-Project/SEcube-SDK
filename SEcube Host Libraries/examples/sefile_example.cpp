/**
  ******************************************************************************
  * File Name          : sefile_example.cpp
  * Description        : Usage example of SEfile APIs.
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

/*! \file  sefile_example.cpp
 *  \brief This is an example to show you how the APIs of SEfile can be used.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>

using namespace std;

// RENAME THIS TO main()
int sefile_example() {
	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	array<uint8_t, L1Parameters::Size::PIN> pin = {'t','e','s','t'};

	cout << "--- SEfile library example ---" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000)); // these delays are simply here to slow down the process making it more comprehensible
	cout << "Looking for SEcube devices..." << endl;
	this_thread::sleep_for(chrono::milliseconds(2000));

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
		cout << "Input error...quit." << endl;
		return -1;
	}

	if((sel >= 0) && (sel < numdevices)){
		std::array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
		if(devices.at(sel).second.length() > L0Communication::Size::SERIAL){
			cout << "Unexpected error...quit." << endl;
			return -1;
		} else {
			memcpy(sn.data(), devices.at(sel).second.data(), devices.at(sel).second.length());
		}
		l1->L1SelectSEcube(sn);
		cout << "Selected device:" << devices.at(sel).first << " - " << devices.at(sel).second << endl;
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
			cout << "SEcube login error. Quit." << endl;
			return 0;
		}
		if(!l1->L1GetSessionLoggedIn()){ // check if login was ok
			cout << "SEcube login error. Quit." << endl;
			return 0;
		} else {
			cout << "SEcube login OK" << endl;
		}


		/* SEfile example */

		vector<pair<uint32_t, uint16_t>> keys;
		try{
			l1->L1KeyList(keys);
		} catch (...) {
			cout << "\nError retrieving keys inside the SEcube device. Quit." << endl;
			l1->L1Logout();
			return -1;
		}
		cout << "\nHere is the list of the keys stored on the SEcube." << endl;
		cout << "Please enter the number of the key that you would like to use in order to run this example." << endl;
		if(keys.size() == 0){
			cout << "\nError, there are no keys inside the SEcube device. Impossible to continue." << endl;
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
		key = keys.at(ch).first;

		string content = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam arcu ex, interdum in dolor quis, semper laoreet dolor.\n"
						 "Etiam eros mauris, interdum sed consectetur sit amet, commodo eget massa. Pellentesque tincidunt ullamcorper rhoncus.\n"
						 "Proin eget congue velit, eu faucibus libero. Mauris cursus in ante sit amet blandit. Fusce sit amet.";
		cout << "\nWe will create an encrypted text file using the APIs of SEfile. This is the content that will be written in the file:" << endl;
		cout << content << endl;

		/* Now we want to create a simple text file encrypted with SEfile. Therefore, instead of using the normal functions provided by the
		 * OS, we will use the functions of SEfile. */
		SEfile file1(l1.get(), key, L1Algorithms::Algorithms::AES_HMACSHA256); // we create a SEfile object bounded to the L1 SEcube object, we specify also the ID of the key and the algorithm that we want to use
		string filename = "sefile_example.txt";

		/* here we simply call the SEfile open function, notice that we pass the SEFILE_WRITE flag because we want to modify the content of the file, then we also
		 * pass the SEFILE_NEWFILE flag because we want to create the file */
		file1.secure_open((char*)filename.c_str(), SEFILE_WRITE, SEFILE_NEWFILE);
		int pos; /* we move to the end of the file (this is not really necessary here because the file has just been created) */
		file1.secure_seek(0, &pos, SEFILE_END); // append to the end of the file
		file1.secure_write((uint8_t*)content.c_str(), content.size()); /* now we simply write our buffer to the file */
		file1.secure_close(); // finally we close the file

		/* Now suppose that we want to read the content of the file we have just created */
		SEfile file2(l1.get()); // we create another SEfile object (we could have used the previous one)
		/* notice that we set only the L1 object for file2, we do not set the key or the algorithm. we could have set also those parameters because we know them but, tipically,
		 * in the real world you don't know which key and which algorithm have been used to encrypt a file protected by SEfile. this is due to the fact that you cannot keep track
		 * of the encryption details of many, many files. for this reason SEfile will automatically understand, from the header of the encrypted file, which key ID and which algorithm
		 * have been used...so there is no need to set them in advance when you simply want to open an existing file (you must do that only when you want to create a file) */
		file2.secure_open((char*)filename.c_str(), SEFILE_READ, SEFILE_OPEN); // notice that we simply read the file and we open it because it already exists
		file2.secure_seek(0, &pos, SEFILE_BEGIN); // not really necessary, when a file is opened its pointer is already set to the first sector (header excluded)
		uint32_t filedim;
		secure_getfilesize((char*)filename.c_str(), &filedim, l1.get()); // here we retrieve the total size of the valid data stored inside the file
		unique_ptr<char[]> filecontent = make_unique<char[]>(filedim);
		unsigned int bytesread;
		file2.secure_read((uint8_t*)filecontent.get(), filedim, &bytesread); // here we simply read the entire file into our buffer
		file2.secure_close(); // we can close our file

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\nThe encrypted text file has been created successfully.\nHere is the content of the encrypted file, as if it was read by "
				"any text editor that cannot use the SEfile APIs to decrypt the content:" << endl;
		char encoded_filename[256]; // we generate the encoded filename and read the encrypted file as a normal file just to show that it is encrypted...this is not required in real applications
		uint16_t enclen = 0;
		memset(encoded_filename, '\0', 256);
		uint16_t r = crypto_filename((char*)filename.c_str(), encoded_filename, &enclen); // generate the name of the encrypted file starting from the original name (encoded filename = SHA-256 of original name)
		if(r){
			cout << "\nUnexpected error. Quit." << endl;
			l1->L1Logout();
			return -1;
		}
		ifstream encfile(encoded_filename, ios::binary|ios::in|ios::ate);
		if(encfile.is_open()){
			streampos size;
			char * memblock;
			size = encfile.tellg();
		    memblock = new char [size];
		    memset(memblock, '\0', size);
		    encfile.seekg (0, ios::beg);
		    encfile.read (memblock, size);
		    encfile.close();
		    string str(memblock, size);
		    delete[] memblock;
		    cout << str << endl;
		} else {
			cout << "\nUnexpected error. Quit." << endl;
			l1->L1Logout();
			return -1;
		}

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\nAs you can see, the content is not the same that we showed at the beginning.\nThis is because the original data (plaintext) is now "
				"encrypted (ciphertext), its real value can be read only using the SEfile APIs, the SEcube and the correct key for decrypting "
				"the data.\nAs a matter of proof, here is the decrypted content of the file, read by the SEfile APIs:" << endl;
		string readcontent(filecontent.get(), filedim);
		cout << readcontent << endl; // we print the content of the text file

		/* End of SEfile example */


		/* Beginning of SEcure Database example based on SEfile and SQLite */
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\n\nBefore finishing this example, we also use very basic SEfile APIs to create an encrypted SQL database managed with SQLite.\n"
				"We will not perform any action on this database, we simply create it and then close it.\nTake a look at the source code of this"
				" example in order to see how it is done.\nOnce the database is opened, you can use it as if it was a normal SQLite database, therefore "
				"you can use the standard SQLite C interface.\nThe database is internally implemented by SEfile, so it is encrypted, but you do not need "
				"to care about these details." << endl;
		sqlite3 *db;
		unique_ptr<SEfile> mydatabase = make_unique<SEfile>(); // notice that we use a standard SEfile object also for encrypted databases
		mydatabase->secure_init(l1.get(), key, L1Algorithms::Algorithms::AES_HMACSHA256); // we initialize the SEfile object with the parameters we want (we could have done this using the specific constructor)
		char dbname[] = "my_encrypted_database";
		memcpy(mydatabase->handleptr->name, dbname, strlen(dbname)); // you MUST copy the name of the database that you want to open/create into the name field of the handleptr attribute of the SEfile object
		databases.push_back(std::move(mydatabase)); // you MUST move the SEfile object to the databases vector
		int rc = sqlite3_open(dbname, &db); // now you can open your database
		if(rc != SQLITE_OK){
			cout << "Error creating the encrypted database!" << endl;
			l1->L1Logout();
			return -1;
		} else {
			cout << "Encrypted SQLite database created!" << endl;
			this_thread::sleep_for(chrono::milliseconds(1000));
			cout << "Encrypted SQLite database successfully opened!" << endl;
			this_thread::sleep_for(chrono::milliseconds(1000));
			cout << "Here you can call any API from the standard SQLite C interface (see SQLite documentation for info)." << endl;
		}
		/* here you can do whatever you want with the database, always using the standard APIs of the SQLite C interface */
		sqlite3_close(db); // finally you close your database
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "Encrypted SQLite database closed." << endl;

		/* End of SEcure Database example */

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\nLogging out..." << endl;
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
		cout << "You are now logged out." << endl;
		cout << "\nExample completed.\nPress q to quit." << endl;
		while(cin.get() != 'q'){};
	} else {
		cout << "You entered an invalid number. Quit." << endl;
		return -1;
	}

	return 0;
}
