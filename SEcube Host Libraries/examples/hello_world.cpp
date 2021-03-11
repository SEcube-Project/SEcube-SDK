/**
  ******************************************************************************
  * File Name          : hello_world.cpp
  * Description        : basic example of login/logout APIs.
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

/*! \file  hello_world.cpp
 *  \brief This file is a simple example of how to use very basic SEcube API (mainly login and logout).
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include "../sources/L1/L1.h"
#include <memory>
#include <thread>
#include <iostream>

using namespace std;

// RENAME THIS TO main()
int helloworld() {
	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	array<uint8_t, L1Parameters::Size::PIN> pin_admin = {'t','e','s','t'}; // PIN to login as administrator on the SEcube (assuming the device was initialized with this PIN)

	cout << "Welcome to SEcube Hello World!" << endl;
	this_thread::sleep_for(chrono::milliseconds(1000)); // these delays are simply here to slow down the process making it more comprehensible
	cout << "Looking for SEcube devices...\n" << endl;
	this_thread::sleep_for(chrono::milliseconds(2000));

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if(ret){
		cout << "Error while searching for SEcube devices! Quit." << endl;
		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if(numdevices == 0){
		cout << "No SEcube devices found! Quit." << endl;
		return 0;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	cout << "List of SEcube devices (path, serial number):" << endl;
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
		cout << "\nDevice " << devices.at(sel).first << " - " << devices.at(sel).second << " selected." << endl;
		cout << "Attempting login..." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		try{
			l1->L1Login(pin_admin, SE3_ACCESS_ADMIN, true); // provide pin, access privilege and boolean to force logout before login (in case there was already an active login on the device)
			/*
			 * notice that when the login is executed, the SEcube device enters a specific loop to handle requests coming from the host.
			 * if you close your program without performing the logout, the SEcube device will still be in that loop the next time you
			 * will attempt the login (provided that you do not unplug the SEcube from the PC, removing power). this is the reason why
			 * you have to provide the boolean as third parameter of the L1Login function, because you need to force the logout from
			 * previous pending sessions. if you do not force the logout, you won't be able to communicate with the SEcube (because the
			 * session is still open on the SEcube side, but it doesn't exist anymore on your host).
			 */
		} catch (...) { // catch any kind of exception (login will throw if the password is wrong or if any error happens)
			cout << "Login error. Quit." << endl;
			return 0;
		}
		if(!l1->L1GetSessionLoggedIn()){ // check if login was ok
			cout << "Login error. Quit." << endl;
			return 0;
		}
		cout << "\nHello World! You are now logged in. You can use the full potential of the SEcube." << endl;
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\nLogging out..." << endl;
		try{
			l1->L1Logout();
		} catch (...) {
			cout << "Logout error. Quit." << endl;
			return 0;
		}
		if(l1->L1GetSessionLoggedIn()){ // check if logout was ok
			cout << "Logout error. Quit." << endl;
			return 0;
		}
		cout << "You are now logged out." << endl;
		cout << "\nBasic SEcube test completed successfully.\nPress q to quit." << endl;
		while(cin.get() != 'q'){};
	} else {
		cout << "You entered an invalid number. Quit." << endl;
		return 0;
	}

	return 0;
}
