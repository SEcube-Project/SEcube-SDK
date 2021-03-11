/**
  ******************************************************************************
  * File Name          : sekey_example.cpp
  * Description        : Usage example of SEkey APIs.
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

/*! \file  sekey_example.cpp
 *  \brief  This is an example to show you how the APIs of SEkey can be used to build the KMS and to manage the keys, the groups and the users.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

/* SEkey is not a "plug-and-play" KMS, it is a collection of APIs that can be used to implement a KMS
 * based on the hardware capabilities of the SEcube device. */

#include <fstream>
#include "../sources/L1/L1.h"
#include "../sekey/SEkey.h"
#include <thread> // thread::sleep_for

using namespace std;

void admin_mode();
void user_mode();
extern userinfo currentuser;

void print_db_content(); /* Example about how to print the content of the SEkey database of the current user (or the administrator). */
int init_user_secube_wrapper(std::string& uid, std::array<uint8_t, L1Parameters::Size::PIN>& userpin, std::array<uint8_t, L1Parameters::Size::PIN>& adminpin); /* Example about how to initialize the SEcube of a user. */
vector<array<uint8_t, L1Parameters::Size::PIN>> getPINs(); /* Commodity function useful to create a vector of possible PINs to be used for login on the SEcube. */

// RENAME THIS TO main()
int sekey_example()
{
	cout << "--- SEkey KMS example ---" << endl;
	cout << "\nDISCLAIMER: this example shows how SEkey APIs can be used.\nThis program will not print almost anything on the console, unless errors occur.\n"
			"Please take a look at the source code of this example in order to understand how the KMS can be used.\nYou will be asked to run the example in admin mode"
			" or in user mode.\nAdmin mode must be run only one time, and only using the SEcube device of the SEkey administrator.\nThe SEcube of the administrator must be erased and "
			"reprogrammed with the latest SEcube firmware before running this example. The same holds for the SEcube devices of the users." << endl;
    while(1){
    	string userinput;
        cout << "\nPress 1 for admin mode (use only with the SEcube of the SEkey admin)" << endl;
        cout << "Press 2 for user mode (use with any SEcube belonging to the SEkey users)" << endl;
        cout << "Press q to quit" << endl;
        getline(cin, userinput);
        char ch = userinput.at(0);
        switch(ch){
    		case '1':
    			admin_mode();
    			break;
    		case '2':
    			user_mode();
    			break;
    		case 'q':
    			return 0;
    		default:
    			cout << "Invalid choice." << endl;
        }
    }
    cout << "Press q to quit the program." << endl;
    while(cin.get() != 'q'){};
    return 0;
}

int init_user_secube_wrapper(string& uid, array<uint8_t, L1Parameters::Size::PIN>& userpin, array<uint8_t, L1Parameters::Size::PIN>& adminpin){
	/* !!! LINUX USERS: when you connect the SEcube of the user to the host computer, remember to mount the SEcube otherwise this API might fail. */
	vector<array<uint8_t, L1Parameters::Size::PIN>> pins = getPINs(); // vector of pins to try for login as admin on user secube
	for(;;){
		cout << "\nInitializing SEcube of a new user with ID " << uid << endl;
		cout << "Please connect the SEcube of the user to the host machine. Press 'n' when ready, press 'q' to quit this operation." << endl;
		string userinput;
		getline(cin, userinput);
		if(userinput.compare("n") == 0){
			int x = sekey_init_user_SEcube(uid, userpin, adminpin, pins);
			for(;;){
				cout << "\nPlease disconnect the SEcube of the user from the host machine. Press 'n' when done." << endl;
				getline(cin, userinput);
				if(userinput.compare("n") == 0){
					unique_ptr<L0> l0 = make_unique<L0>();
					if(l0->GetNumberDevices() == 1){
						return x;
					}
				}
			}
		}
		if(userinput.compare("q") == 0){
			return 0;
		}
	}
	return 0; // this is here just to avoid the warning from the compiler (actual return will be done inside the infinite loop)
}

void print_db_content(){
	vector<se_user> users;
	vector<se_group> groups;
	vector<se_key> keys;
	ofstream outputfile;
	string fname = currentuser.userid + '_' + "dump.txt"; // i.e. U1_dump.txt
	outputfile.open (fname.c_str());
	// print list of users
	if(sekey_user_get_info_all(&users) != SEKEY_OK){ return; }
	for(se_user& user : users){	user.print_user_details(outputfile); }
	// print list of groups
	if(sekey_group_get_info_all(&groups) != SEKEY_OK){ return; }
	for(se_group& group : groups){ group.print_group_details(outputfile); }
	// print list of keys
	if(sekey_key_get_info_all(&keys) != SEKEY_OK){ return; }
	for(se_key& key : keys){ key.print_key_details(outputfile); }
	outputfile.close();
}

vector<array<uint8_t, L1Parameters::Size::PIN>> getPINs(){
	vector<array<uint8_t, L1Parameters::Size::PIN>> pins;
	pins.push_back(array<uint8_t, L1Parameters::Size::PIN>({'u','s','e','r'})); // PIN = "user" (+ 0s up to 32 byte)
	pins.push_back(array<uint8_t, L1Parameters::Size::PIN>({'a','d','m','i','n'})); // PIN = "admin" (+ 0s up to 32 byte)
	pins.push_back(array<uint8_t, L1Parameters::Size::PIN>({'t','e','s','t'})); // PIN = "test" (+ 0s up to 32 byte)
	pins.push_back(array<uint8_t, L1Parameters::Size::PIN>({})); // default PIN is all zero
	return pins;
}

/* admin_mode() simulates the interaction of a real administrator with the KMS */
void admin_mode(){
	cout << "\nGENERAL WARNING: running this example may take a lot of time (about 30 min) because of many SEkey API calls.\n"
			"The program may appear to be stuck, but if you check the content of the SEcube MicroSD card you will see that"
			" there is a lot of activity going on!\nAny error happening while executing SEkey APIs will be reported on the console." << endl;

	cout << "\nPERFORMANCE WARNING: SEkey was not developed having performance in mind.\nUsing the MicroSD card of the SEcube and a "
			"fully encrypted database to manage the metadata of the KMS has a huge negative impact on the performance of the system.\n"
			"If you want, you are free to improve it and modify it as you wish!" << endl;

	cout << "\nThis example will generate a text file with the dump of the metadata of the KMS in a file called 'admin_dump.txt'." << endl;

	/* do not mind to these variables...they are here simply to run the example */
	string gid, gname, uid, uname, kid, kname, h1, h2;
	int iteration = 0;
	group_policy gpolicy;
	sekey_error rc;

	/* this is a list of pins that may be used to perform the login (with admin or user privilege) on any SEcube.
	 * notice that in a real-world scenario the software should ask for the pin to the user of the PC (by means
	 * of a GUI or a CLI); here we use a hard-coded list for simplicity.  */
	vector<array<uint8_t, L1Parameters::Size::PIN>> pins = getPINs();

	/* PIN = "test" for both user privilege level and admin privilege level.
	 * notice that we use this PIN for sake of simplicity, the real application must not use hard-coded PINs and it must
	 * use complex PINs. it should generate the PINs automatically as a random 32 byte array or it should ask to the administrator
	 * to input the PINs on the keyboard. notice also that every SEcube must be initialized with an "admin PIN" and a "user PIN",
	 * these values should be different for every SEcube. the KMS automatically keeps track of the PINs of all SEcube(s), but these
	 * are visible only in the SEcube of the administrator. on the contrary, the users are provided exclusively with the "user PIN"
	 * of their SEcube. */
	array<uint8_t, L1Parameters::Size::PIN> pin = {'t','e','s','t'}; // PIN = "test" (+ 0s up to 32 byte)

	/* L0 objects are required to communicate at the low-level (i.e. data over USB cable) with the SEcube.
	 * Notice that a smart pointer is used to incapsulate the L0 object, this is useful to avoid memory
	 * leakage that may happen if using the native object (you can use a unique pointer or a shared pointer
	 * depending on your needs). */
	unique_ptr<L0> l0 = make_unique<L0>();
	if(l0->GetNumberDevices() != 1){
		cout << "\nAdmin SEcube not connected or too many SEcube devices connected. Quit." << endl;
		return;
	}

	/* L1 objects are used to perform high-level operations with the SEcube. Use the L1 object incapsulated by a unique ptr
	 * (or shared ptr) because the smart pointer allows to call automatically the destructor of the L1 object. */
	unique_ptr<L1> l1 = make_unique<L1>();

	try{
		l1->L1Login(pin, SE3_ACCESS_ADMIN, true); // if the login fails, L1Login throws an exception
		cout << "\nSEcube login OK!" << endl;
	} catch(...){
		try{
			array<uint8_t, L1Parameters::Size::PIN> emptypin = {0}; // in case the SEcube has not been initialized yet
			l1->L1Login(emptypin, SE3_ACCESS_ADMIN, true); // if the login fails, L1Login throws an exception
			cout << "\nSEcube login OK!" << endl;
		} catch(...){
			cout << "\nSEcube login error. Quit." << endl;
			return;
		}
	}

	if(!l1->L1SEkey_isReady()){ // check if the SEcube is already initialized for SEkey
		try{
			for(;;){
				/* this is an example about how to initialize the SEcube of the administrator. notice that the interaction with the admin, using the host PC,
				 * should be done according to the software you want to develop (i.e. GUI or CLI). here we simply print questions on the terminal; you should
				 * follow the same pattern in your software (i.e. add a button to the GUI to initialize the SEcube of the administrator). */
				cout << "The SEcube doesn't seem to be initialized. Do you want to initialize it as administrator SEcube for SEkey? (y/n)" << endl;
				string userinput;
				getline(cin, userinput);
				if(userinput.compare("y") == 0){
					/* here we call the API to initialize the SEcube of the administrator. notice that we pass the L1 object and the PINs to be set. */
					if(sekey_admin_init(*l1, pins, pin, pin) != SEKEY_OK){
						// if something goes wrong erase the SEcube (because the flash memory may be dirty) and try again.
						cout << "\nFATAL ERROR while initializing the SEcube. Please erase the flash of the SEcube and program the firmware again, then retry." << endl;
						return;
					} else {
						cout << "Admin init OK!" << endl;
						/* notice how we reset the smart pointer of L0 and L1 (so that we avoid memory leakage). this must be done because, after the initialization,
						 * the SEcube must be disconnected from the PC and reconnected in order to make the changes effective. */
						l1.reset(); // L1 destructor is called
						l0.reset(); // L0 destructor is called
						for(;;){
							string usrinput;
							cout << "\nDisconnect and reconnect the administrator SEcube...press 'n' then enter when done.";
							getline(cin, usrinput);
							if(usrinput.compare("n") == 0){
								l0 = make_unique<L0>(); // notice how we create again the L0 object (incapsulated by a unique ptr)
								if(l0->GetNumberDevices() == 1){ // again, check the number of devices
									break;
								} else {
									l0.reset(); // if the number of devices is not exactly 1 (meaning no secube connected or too many secube connected) reset the pointer in order to call the L0 destructor
								}
							}
						}
						l1 = make_unique<L1>(); // L1 must be recreated because the SEcube was disconnected and reconnected, same pattern of L0
						try{
							l1->L1Login(pin, SE3_ACCESS_ADMIN, true); // now we can login with the admin PIN that we set
						} catch(...) {
							cout << "\nERROR trying to login after SEcube reconnection. Quit." << endl;
							return;
						}
						break;
					}
				}
				if(userinput.compare("n") == 0){
					cout << "Quit because the SEcube is not initialized." << endl;
					return;
				}
			}
		} catch (...){ // if something goes wrong erase the SEcube (because the flash memory may be dirty) and try again.
			cout << "\nFATAL ERROR while initializing the SEcube. Please erase the flash of the SEcube and program the firmware again, then retry." << endl;
			return;
		}
	}

	/* Start SEkey, providing the required L0 and L1 objects (they are used as global variables by the APIs) */
	cout << "Starting SEkey..." << endl;
	if(sekey_start(*l0, l1.get()) != 0){
		cout << "\nERROR - Impossible to start SEkey." << endl;
		return;
	}
	cout << "SEkey started!" << endl;

	/* Now we will perform several operations on SEkey in order to show how the KMS works.
	 * Each operation is meant to test a specific API, the order of the operations is important
	 * to check that the system works properly. Some operations are intentionally wrong (i.e. wrong
	 * parameters or changes not allowed like activating a compromised key), after each API call the
	 * database content is printed to a txt file so that you can check it out.
	 * The return value of each API is checked against the expected value, if the returned value does not
	 * match, a log message is printed on the console. */

	/* ADD GROUPS 1,2,3,4,5 */
	cout << "Adding groups..." << endl;
	iteration = 0;
	vector<pair<string,string>> groups;
	groups.push_back(pair<string,string>("G1", "Group_1")); // we send hard-coded parameters to the APIs...this is just to simplify things
	groups.push_back(pair<string,string>("G2", "Group_2"));
	groups.push_back(pair<string,string>("G3", "Group_3"));
	groups.push_back(pair<string,string>("G4", "Group_4"));
	groups.push_back(pair<string,string>("G5", "Group_5"));
	for(pair<string,string> p : groups){
		gid = p.first;
		gname = p.second;
		gpolicy = group_policy(1000, L1Algorithms::Algorithms::AES_HMACSHA256, 31536000); // check out the meaning of policy
		rc = (sekey_error)sekey_add_group(gid, gname, gpolicy); // API
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
		print_db_content();
	}

	/* ADD USERS 1,2,3,4,5 */
	cout << "Adding users..." << endl;
	iteration = 0;
	vector<pair<string,string>> users;
	users.push_back(pair<string,string>("U1", "User_1"));
	users.push_back(pair<string,string>("U2", "User_2"));
	users.push_back(pair<string,string>("U3", "User_3"));
	users.push_back(pair<string,string>("U4", "User_4"));
	users.push_back(pair<string,string>("U5", "User_5"));
	for(pair<string,string> p : users){
		uid = p.first;
		uname = p.second;
		rc = (sekey_error)sekey_add_user(uid, uname);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
		print_db_content();
	}

	/* !!! the next 4 API calls should be issued only once, when you physically want to initialize the SEcube devices of the users.
	 * Otherwise, simply comment the code. */
	/* INITIALIZE USER 1 */
	uid = "U1";
	rc = (sekey_error)init_user_secube_wrapper(uid, pin, pin);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	} else {
		cout << uid << " done." << endl;
	}
	/* INITIALIZE USER 2 */
	uid = "U2";
	rc = (sekey_error)init_user_secube_wrapper(uid, pin, pin);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	} else {
		cout << uid << " done." << endl;
	}
	/* INITIALIZE USER 3 */
	uid = "U3";
	rc = (sekey_error)init_user_secube_wrapper(uid, pin, pin);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	} else {
		cout << uid << " done." << endl;
	}
	/* INITIALIZE USER 4 */
	uid = "U4";
	rc = (sekey_error)init_user_secube_wrapper(uid, pin, pin);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	} else {
		cout << uid << " done." << endl;
	}

	/* ADD USERS TO GROUPS */
	cout << "Adding users to groups..." << endl;
	iteration = 0;
	vector<pair<string,string>> usergroups;
	usergroups.push_back(pair<string,string>("U1", "G1")); // U1, U2, U3, U4, U5 in Group 1
	usergroups.push_back(pair<string,string>("U2", "G1"));
	usergroups.push_back(pair<string,string>("U3", "G1"));
	usergroups.push_back(pair<string,string>("U4", "G1"));
	usergroups.push_back(pair<string,string>("U5", "G1"));
	usergroups.push_back(pair<string,string>("U1", "G2")); // U1, U2, U3, U4 in Group 2
	usergroups.push_back(pair<string,string>("U2", "G2"));
	usergroups.push_back(pair<string,string>("U3", "G2"));
	usergroups.push_back(pair<string,string>("U4", "G2"));
	usergroups.push_back(pair<string,string>("U1", "G3")); // U1, U2, U3, U5 in Group 3
	usergroups.push_back(pair<string,string>("U2", "G3"));
	usergroups.push_back(pair<string,string>("U3", "G3"));
	usergroups.push_back(pair<string,string>("U5", "G3"));
	usergroups.push_back(pair<string,string>("U1", "G4")); // U1, U2, U4, U5 in Group 4
	usergroups.push_back(pair<string,string>("U2", "G4"));
	usergroups.push_back(pair<string,string>("U4", "G4"));
	usergroups.push_back(pair<string,string>("U5", "G4"));
	usergroups.push_back(pair<string,string>("U1", "G5")); // U1, U3, U4, U5 in Group 5
	usergroups.push_back(pair<string,string>("U3", "G5"));
	usergroups.push_back(pair<string,string>("U4", "G5"));
	usergroups.push_back(pair<string,string>("U5", "G5"));
	for(pair<string,string> p : usergroups){
		uid = p.first;
		gid = p.second;
		rc = (sekey_error)sekey_add_user_group(uid, gid);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
		print_db_content();
	}

	/* CREATE KEYS */
	cout << "Adding keys..." << endl;
	iteration = 0;
	vector<pair<int,string>> keys;
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+1, "G1")); // ID = 2001
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+2, "G2")); // ID = 2002
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+3, "G3")); // ID = 2003
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+4, "G4")); // ID = 2004
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+5, "G5")); // ID = 2005
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+6, "G1")); // ID = 2006
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+7, "G2")); // ID = 2007
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+8, "G3")); // ID = 2008
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+9, "G4")); // ID = 2009
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+10, "G5")); // ID = 2010
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+11, "G1")); // ID = 2011
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+12, "G2")); // ID = 2012
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+13, "G3")); // ID = 2013
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+14, "G4")); // ID = 2014
	keys.push_back(pair<int,string>(L1Key::Id::SEKEY_ID_BEGIN+15, "G5")); // ID = 2015
	for(pair<int,string> p : keys){
		kid = "K" + to_string(p.first);
		kname = "Key_" + to_string(p.first);
		gid = p.second;
		rc = (sekey_error)sekey_add_key(kid, kname, gid, 0, se_key_type::symmetric_data_encryption);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
		print_db_content();
	}

	/* ACTIVATE KEYS */
	cout << "Activating keys..." << endl;
	iteration = 0;
	vector<int> toactivate = {L1Key::Id::SEKEY_ID_BEGIN+1, L1Key::Id::SEKEY_ID_BEGIN+2, L1Key::Id::SEKEY_ID_BEGIN+3, L1Key::Id::SEKEY_ID_BEGIN+4, L1Key::Id::SEKEY_ID_BEGIN+5};
	for(int k : toactivate){ // 2001, 2002, 2003, 2004, 2005
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_activate_key(kid);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* DESTROY KEYS */
	cout << "Doing more operations on the KMS..." << endl;
	iteration = 0;
	vector<int> todestroy = {L1Key::Id::SEKEY_ID_BEGIN+6, L1Key::Id::SEKEY_ID_BEGIN+13, L1Key::Id::SEKEY_ID_BEGIN+15};
	for(int k : todestroy){ // 2006, 2013, 2015
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_key_change_status(kid, se_key_status::destroyed);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* COMPROMISE KEYS */
	iteration = 0;
	vector<int> tocomp = {L1Key::Id::SEKEY_ID_BEGIN+1, L1Key::Id::SEKEY_ID_BEGIN+5}; // 2001, 2005
	for(int k : tocomp){
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_key_change_status(kid, se_key_status::compromised);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* DEACTIVATE KEYS */
	iteration = 0;
	vector<int> todeactivate = {L1Key::Id::SEKEY_ID_BEGIN+9, L1Key::Id::SEKEY_ID_BEGIN+14}; // 2009, 2014
	for(int k : todeactivate){
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_key_change_status(kid, se_key_status::deactivated);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* SUSPEND KEY 2003 */
	kid = "K2003";
	rc = (sekey_error)sekey_key_change_status(kid, se_key_status::suspended); // 2003
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ACTIVATE KEYS */
	iteration = 0;
	toactivate.clear();
	toactivate = {L1Key::Id::SEKEY_ID_BEGIN+7, L1Key::Id::SEKEY_ID_BEGIN+8, L1Key::Id::SEKEY_ID_BEGIN+10}; // 2007, 2008, 2010
	for(int k : toactivate){
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_activate_key(kid);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* SUSPEND KEY 2007 */
	kid = "K2007";
	rc = (sekey_error)sekey_key_change_status(kid, se_key_status::suspended); // 2007
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ACTIVATE KEY 2011 */
	kid = "K2011";
	rc = (sekey_error)sekey_activate_key(kid); // 2011
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ACTIVATE KEY 2009 (already deactivated) */
	kid = "K2009";
	rc = (sekey_error)sekey_activate_key(kid); // 2009
	if(rc != SEKEY_ERR){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ACTIVATE KEY 2001 (already compromised) */
	kid = "K2001";
	rc = (sekey_error)sekey_activate_key(kid); // 2001
	if(rc != SEKEY_ERR){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ACTIVATE KEY 2013 (already destroyed) */
	kid = "K2013";
	rc = (sekey_error)sekey_activate_key(kid); // 2013
	if(rc != SEKEY_ERR){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* SET TO KEYS PREACTIVE (never allowed) */
	iteration = 0;
	vector<int> topreactive = {105, 107, 108};
	for(int k : topreactive){
		kid = "K" + to_string(k);
		rc = (sekey_error)sekey_key_change_status(kid, se_key_status::preactive);
		if(rc != SEKEY_ERR_PARAMS){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* CHANGE KEYS NAME */
	iteration = 0;
	vector<int> torename = {L1Key::Id::SEKEY_ID_BEGIN+1, L1Key::Id::SEKEY_ID_BEGIN+2, L1Key::Id::SEKEY_ID_BEGIN+3};
	for(int k : torename){
		kid = "K" + to_string(k);
		kname = "KEY" + to_string(k);
		rc = (sekey_error)sekey_key_change_name(kid, kname);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* CHANGE GROUPS NAME */
	iteration = 0;
	torename.clear();
	torename = {1, 2, 3, 4};
	for(int k : torename){
		gid = "G" + to_string(k);
		gname = "GROUP" + to_string(k);
		rc = (sekey_error)sekey_group_change_name(gid, gname);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* CHANGE GROUPS MAX KEYS */
	iteration = 0;
	torename.clear();
	torename = {1, 2, 3, 4};
	for(int k : torename){
		gid = "G" + to_string(k);
		rc = (sekey_error)sekey_group_change_max_keys(gid, 9999);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* CHANGE GROUP DEFAULT CRYPTOPERIOD */
	iteration = 0;
	torename.clear();
	torename = {1, 2, 3, 4};
	for(int k : torename){
		gid = "G" + to_string(k);
		rc = (sekey_error)sekey_group_change_default_cryptoperiod(gid, 2629800);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* CHANGE USER NAMES */
	iteration = 0;
	torename.clear();
	torename = {1, 2, 3};
	for(int k : torename){
		uid = "U" + to_string(k);
		uname = "USER" + to_string(k);
		rc = (sekey_error)sekey_user_change_name(uid, uname);
		if(rc != SEKEY_OK){
			cout << "Error at line " << __LINE__ << " during iteration " << iteration << endl;
		}
		iteration++;
	}

	/* ADD USER 5 GROUP 2 */
	uid = "U5";
	gid = "G2";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* REMOVE USER 1 GROUP 3 */
	uid = "U1";
	gid = "G3";
	rc = (sekey_error)sekey_delete_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* REMOVE USER 5 GROUP 2 */
	uid = "U5";
	gid = "G2";
	rc = (sekey_error)sekey_delete_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* REMOVE USER 4 GROUP 3 (must return unchanged because user 4 does not belong to group 3) */
	uid = "U4";
	gid = "G3";
	rc = (sekey_error)sekey_delete_user_group(uid, gid);
	if(rc != SEKEY_UNCHANGED){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* REMOVE USER 5 GROUP 2 (must return unchanged because user 5 does not belong to group 2) */
	uid = "U5";
	gid = "G2";
	rc = (sekey_error)sekey_delete_user_group(uid, gid);
	if(rc != SEKEY_UNCHANGED){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* DELETE USER 1 */
	uid = "U1";
	rc = (sekey_error)sekey_delete_user(uid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* DELETE USER 1 (already deleted) */
	uid = "U1";
	rc = (sekey_error)sekey_delete_user(uid);
	if(rc != SEKEY_USER_NOT_FOUND){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* DELETE USER THAT DOES NOT EXIST */
	uid = "U123456";
	rc = (sekey_error)sekey_delete_user(uid);
	if(rc != SEKEY_USER_NOT_FOUND){
		cout << "Error at line " << __LINE__ << endl;
		cin.get();
	}

	/* DELETE GROUP 4 */
	gid = "G4";
	rc = (sekey_error)sekey_delete_group(gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* DELETE GROUP 5 */
	gid = "G5";
	rc = (sekey_error)sekey_delete_group(gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* RECREATE USER 1 */
	uid = "U1";
	uname = "USER_1";
	rc = (sekey_error)sekey_add_user(uid, uname);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD USER 1 TO GROUP 1 */
	uid = "U1";
	gid = "G1";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	// change name of user that does not exist
	uid = "U123456";
	uname = "user_123456";
	rc = (sekey_error)sekey_user_change_name(uid, uname);
	if(rc != SEKEY_USER_NOT_FOUND){
		cout << "Error at line " << __LINE__ << endl;
		cin.get();
	}

	/* ADD USER 1 TO GROUP 2 */
	uid = "U1";
	gid = "G2";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* DELETE USER 2 */
	uid = "U2";
	rc = (sekey_error)sekey_delete_user(uid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD ANOTHER ACTIVE KEY TO GROUP 2 */
	kid = "K2031";
	kname = "Key_2031";
	gid = "G2";
	rc = (sekey_error)sekey_add_key(kid, kname, gid, 1440, se_key_type::symmetric_data_encryption);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}
	rc = (sekey_error)sekey_activate_key(kid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD USER 5 TO GROUP 2 */
	uid = "U5";
	gid = "G2";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD GROUP 4 */
	gid = "G4";
	gname = "GROUP_4";
	gpolicy = group_policy(1000, L1Algorithms::Algorithms::AES_HMACSHA256, 31536000);
	rc = (sekey_error)sekey_add_group(gid, gname, gpolicy);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD USER 1 TO GROUP 4 */
	uid = "U1";
	gid = "G4";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD USER 2 TO GROUP 4 */
	uid = "U2";
	gid = "G4";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_USER_NOT_FOUND){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* RECREATE USER 2 */
	uid = "U2";
	uname = "USER_2";
	rc = (sekey_error)sekey_add_user(uid, uname);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD USER 2 TO GROUP 4 */
	uid = "U2";
	gid = "G4";
	rc = (sekey_error)sekey_add_user_group(uid, gid);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* ADD TWO KEYS TO GROUP 4 */
	kid = "K2020";
	kname = "Key_2020";
	gid = "G4";
	rc = (sekey_error)sekey_add_key(kid, kname, gid, 1440, se_key_type::symmetric_data_encryption);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}
	kid = "K2021";
	kname = "Key_2021";
	gid = "G4";
	rc = (sekey_error)sekey_add_key(kid, kname, gid, 1440, se_key_type::symmetric_data_encryption);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	/* REQUEST RECOVERY FOR USER 2 */
	se_user su;
	uid = "U2";
	if(sekey_user_get_info(uid, &su) != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}
	string sn = su.get_sn();
	rc = (sekey_error)sekey_recovery_request(uid, sn);
	if(rc != SEKEY_OK){
		cout << "Error at line " << __LINE__ << endl;
	}

	// end the admin mode
	print_db_content();
	sekey_stop();
	cout << "SEkey stopped. Admin mode completed." << endl;
	l1->L1Logout();
	return;
}

/* here we simulate the behavior of a user */
void user_mode(){
	cout << "\nThis example will generate a text file with the dump of the metadata of the KMS in a file whose name will contain the ID of the user (i.e. U1_dump.txt)." << endl;

	/* these variables are used only to run the example */
	string gid, gname, uid, uname, kid, kname;
	group_policy gpolicy;
	sekey_error rc;
	vector<se_user> users;
	string dest, h1, h2, chosen;
	string source;

	unique_ptr<L0> l0 = make_unique<L0>(); // use smart ptr to avoid memory leakage (check comment on admin function above)
	if(l0->GetNumberDevices() != 1){
		cout << "\nSEcube not connected or too many SEcube devices connected. Going back to main menu." << endl;
		return;
	}

	array<uint8_t, L1Parameters::Size::PIN> pin_user = {'t','e','s','t'}; // PIN = "test" (+ 0s up to 32 byte)
	unique_ptr<L1> l1 = make_unique<L1>(); // use smart ptr to avoid memory leakage (check comment on admin function above)
	try{
		l1->L1Login(pin_user, SE3_ACCESS_USER, true);
	} catch(...){
		try{
			array<uint8_t, L1Parameters::Size::PIN> emptypin = {0};
			l1->L1Login(emptypin, SE3_ACCESS_USER, true);
			cout << "\nLogged in with all 0s PIN. This is the default PIN of any SEcube device.\nThe all 0s PIN means "
					"that the SEcube has not been initialized with a sufficiently complex PIN\nor that it has not been "
					"initialized at all.\nContact the SEkey administrator in order to re-initialize the SEcube."
					" Going back to main menu." << endl;
			return;
		}catch(...){
			cout << "\nCannot login with the provided PIN or with all 0s PIN\nCheck with the SEkey administrator if this"
					" SEcube has been correctly initialized for SEkey. Going back to main menu." << endl;
			return;
		}
	}

	if(!l1->L1SEkey_isReady()){ // check if the SEcube is already initialized for SEkey
		cout << "\nThis SEcube has not been initialized yet for SEkey. Please contact the SEkey administrator.\nGoing back to main menu." << endl;
		return;
	}

	cout << "Starting SEkey..." << endl;
	if(sekey_start(*l0, l1.get()) != 0){ // start SEkey
		cout << "Error starting SEkey. Going back to main menu." << endl;
		return;
	}
	cout << "SEkey started." << endl;
	source = currentuser.userid;

	/* explicit request to update the data of SEkey for the user (you don't really need to do this, it is already done automatically) */
	cout << "Updating SEkey user data..." << endl;
	rc = (sekey_error)sekey_update_userdata();
	if(rc == SEKEY_OK){
		cout << "SEkey user data successfully updated." << endl;
		print_db_content();
	} else {
		cout << "Update failed! SEkey functionalities are blocked until the update will be completed successfully.\nGoing back to main menu." << endl;
		return;
	}

	/* TEST KEY GET INFO API: we test both APIs with a trick, first we retrieve all info of all keys with the sekey_key_get_info_all(), then we
	 * use only the IDs of the keys to retrieve the info of each specific key. This is not required in the real-world application, if you already
	 * know the ID of the key that you need, simply use sekey_key_get_info(). Notice that sekey_key_get_info_all() is used when you want to
	 * retrieve general info or when you want to perform some kind of aggregation on the data (i.e. counting how many keys there are, counting how
	 * many keys belong to a certain group, counting the number of active keys, etc.). */
	vector<se_key> keys;
	if(sekey_key_get_info_all(&keys) != 0){
		cout << "Error retrieving the details about all keys of the KMS." << endl;
	}
	for(se_key& key : keys){
		se_key k;
		if(sekey_key_get_info(key.get_id(), &k) != SEKEY_OK){
			cout << "Error at line " << __LINE__ << endl;
		} else {
			ofstream keyinfo("keygetinfo.txt", fstream::out | fstream::app);
			k.print_key_details(keyinfo);
			keyinfo.close();
		}
	}

	/* TEST GROUP GET INFO API. Notice that we use the same trick explained above for the keys. */
	vector<se_group> groups;
	if(sekey_group_get_info_all(&groups) != 0){
		cout << "Error retrieving the details about all groups of the KMS." << endl;
	}
	for(se_group& gr : groups){
		se_group g;
		if(sekey_group_get_info(gr.get_id(), &g) != SEKEY_OK){
			cout << "Error at line " << __LINE__ << endl;
		} else {
			ofstream groupinfo("groupgetinfo.txt", fstream::out | fstream::app);
			g.print_group_details(groupinfo);
			groupinfo.close();
		}
		dest = g.get_id();
		chosen.clear();
		/* here we test the API that retrieves the most secure key to be used in a communication between the user
		 * and the members of a group */
		rc = (sekey_error)sekey_find_key_v2(chosen, dest, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + ", " + dest + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + ", " + dest + " not found. Returned value: " << rc << endl;
		}
	}

	/* TEST USER GET INFO API. Same trick used for keys APIs above. */
	if(sekey_user_get_info_all(&users) != 0){
		cout << "Error retrieving the details about all users of the KMS." << endl;
	}
	for(se_user& us : users){
		se_user u;
		if(sekey_user_get_info(us.get_id(), &u) != SEKEY_OK){
			cout << "Error at line " << __LINE__ << endl;
		} else {
			ofstream userinfo("usergetinfo.txt", fstream::out | fstream::app);
			u.print_user_details(userinfo);
			userinfo.close();
		}
	}

	/* FIND SYMMETRIC KEY FOR EACH COUPLE OF USERS */
	for(se_user& user : users){
		dest = user.get_id();
		rc = (sekey_error)sekey_find_key_v1(chosen, dest, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + ", " + dest + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + ", " + dest + " not found. Returned value: " << rc << endl;
		}
	}

	/* now we simulate, using hard-coded values, the search for the most secure key to be used in specific
	 * combinations like (U1, U2), (U1, U2, U5), etc. */
	vector<string> dest_;
	if(currentuser.userid.compare("U1") == 0){
		dest_.clear();
		dest_.push_back("U4");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U2");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U4");
		dest_.push_back("U3");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
	}
	if(currentuser.userid.compare("U2") == 0){
		dest_.clear();
		dest_.push_back("U1");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
	}
	if(currentuser.userid.compare("U3") == 0){
		dest_.clear();
		dest_.push_back("U1");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U4");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U1");
		dest_.push_back("U4");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
	}
	if(currentuser.userid.compare("U4") == 0){
		dest_.clear();
		dest_.push_back("U1");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U3");
		dest_.push_back("U5");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U1");
		dest_.push_back("U3");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
		dest_.clear();
		dest_.push_back("U2");
		dest_.push_back("U3");
		rc = (sekey_error)sekey_find_key_v3(chosen, dest_, se_key_type::symmetric_data_encryption);
		if(rc == SEKEY_OK){
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + ": " + chosen << endl;
		} else {
			cout << "Key for " + source + " " + dest_[0] + " " + dest_[1] + " not found. Returned value: " << rc << endl;
		}
	}

	// stop SEkey and logout
	print_db_content();
	sekey_stop();
	cout << "SEkey stopped." << endl;
	l1->L1Logout();
	return;
}
