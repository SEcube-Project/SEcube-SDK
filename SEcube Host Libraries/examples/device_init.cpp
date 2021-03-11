/**
  ******************************************************************************
  * File Name          : device_init.cpp
  * Description        : example of SEcube device initialization.
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

/*! \file  device_init.cpp
 *  \brief This file is an example about how to initialize a SEcube device.
 *  \date 17/11/2020
 *  \version SEcube SDK 1.5.1
 */

#include "../sources/L1/L1.h"
#include <iostream>
#include <thread>
#include <memory>

using namespace std;

// RENAME THIS TO main()
int device_init() {
	/* we recommend using smart pointers to manage L0 and L1 objects in order
	   to ensure proper memory management by their constructors and destructors. */
	unique_ptr<L0> l0 = make_unique<L0>();
	unique_ptr<L1> l1 = make_unique<L1>();

	// when the device has not been initialized yet, the default PIN of the SEcube is all-zeros
	array<uint8_t, L1Parameters::Size::PIN> default_pin = {0};

	// the device will be initialized with this PIN ("test" followed by 0s up to 32 bytes in size) for both privilege levels, user and administrator
	array<uint8_t, L1Parameters::Size::PIN> pin = {'t','e','s','t'};

    // the device will be initialized with serial number = "SEcube1234" + '0's up to 32 bytes (remember that the serial number is made only of alnum char, so uint8_t value must be alnum)
	array<uint8_t, L0Communication::Size::SERIAL> serial_number = {'S', 'E', 'c', 'u', 'b', 'e', '1', '2', '3', '4'};
	for(int j=0; j<L0Communication::Size::SERIAL; j++){
		if(!isalnum(serial_number[j])){
			serial_number[j] = '0';
		}
	}
	array<uint8_t, L0Communication::Size::SERIAL> empty_serial_number = {0}; // after erase, the serial number is 0x00 for each byte (32 in total)

	cout << "Welcome to SEcube device initialization example!" << endl;
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
		if(p.second.empty() || memcmp(p.second.data(), empty_serial_number.data(), L0Communication::Size::SERIAL)==0){
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
		if(memcmp(devices.at(sel).second.data(), empty_serial_number.data(), L0Communication::Size::SERIAL)==0){ // if we are looking for a SEcube with an empty SN we use the integer as index to identify the device
			l1->L1SelectSEcube(sel); // this entire part related to the selection of the device must be implemented only if you plan to connect more than 1 SEcube at a time
		} else {
			std::array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
			if(devices.at(sel).second.length() > L0Communication::Size::SERIAL){
				cout << "Unexpected error...quit." << endl;
				return -1;
			} else {
				memcpy(sn.data(), devices.at(sel).second.data(), devices.at(sel).second.length());
			}
			l1->L1SelectSEcube(sn);
		}
		if(devices.at(sel).second.empty() || memcmp(devices.at(sel).second.data(), empty_serial_number.data(), L0Communication::Size::SERIAL)==0){
			cout << "\nDevice " << devices.at(sel).first << " selected." << endl;
		} else {
			cout << "\nDevice " << devices.at(sel).first << " - " << devices.at(sel).second << " selected." << endl;
		}
		cout << "Starting device initialization..." << endl;
		this_thread::sleep_for(chrono::milliseconds(2000));
		try{
			l1->L1FactoryInit(serial_number); // set the serial number
			cout << "SEcube serial number correctly set." << endl;
		} catch (L0FactoryInitException& e){
			cout << e.what() << endl;
			return -1;
		} catch (DeviceAlreadyInitializedException& e){
			cout << e.what() << endl;
			return -1;
		}
		catch (...) {
			cout << "Unexpected error setting the SEcube serial number. SEcube not initialized. Quit." << endl;
			return -1;
		}
		try{
			l1->L1Login(default_pin, SE3_ACCESS_ADMIN, true); // provide pin, access privilege and boolean to force logout before login (in case there was already an active login on the device)
			/*
			 * notice that when the login is executed, the SEcube device enters a specific loop to handle requests coming from the host.
			 * if you close your program without performing the logout, the SEcube device will still be in that loop the next time you
			 * will attempt the login (provided that you do not unplug the SEcube from the PC, removing power). this is the reason why
			 * you have to provide the boolean as third parameter of the L1Login function, because you need to force the logout from
			 * previous pending sessions. if you do not force the logout, you won't be able to communicate with the SEcube (because the
			 * session is still open on the SEcube side, but it doesn't exist anymore on your host).
			 */
		} catch (...) { // catch any kind of exception (login will throw if the password is wrong or if any error happens)
			cout << "Unexpected error during SEcube login. SEcube not initialized. Quit." << endl;
			return 0;
		}
		if(!l1->L1GetSessionLoggedIn()){ // check if login was ok
			cout << "Unexpected error during SEcube login. SEcube not initialized. Quit." << endl;
			return -1;
		}
		try{
			l1->L1SetUserPIN(pin);	// set user pin to "test"
			l1->L1SetAdminPIN(pin); // set admin pin to "test"
			cout << "SEcube PINs correctly set to 'test'." << endl;
			cout << "\nSEcube initialization completed successfully." << endl;
		} catch (...) {
			cout << "Error setting PINs. SEcube not initialized. Quit." << endl;
			return -1;
		}
		try{
			this_thread::sleep_for(chrono::milliseconds(1000));
			cout << "Re-logging to the SEcube to check if new PINs work..." << endl;
			l1->L1Logout();
			l1->L1Login(pin, SE3_ACCESS_ADMIN, true);
			l1->L1Logout();
			l1->L1Login(pin, SE3_ACCESS_USER, true);
			l1->L1Logout();
			this_thread::sleep_for(chrono::milliseconds(1000));
			cout << "Everything is OK!" << endl;
		} catch (...) {
			cout << "Re-logging error. Quit." << endl;
			return 0;
		}
		cout << "\nSEcube initialization example completed successfully.\nPress q to quit." << endl;
		while(cin.get() != 'q'){};
	} else {
		cout << "You entered an invalid number. Quit." << endl;
		return 0;
	}
	return 0;
}
