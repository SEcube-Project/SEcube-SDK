/**
  ******************************************************************************
  * File Name          : L0_base.h
  * Description        : Prototypes of the L0Base library.
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

/*! \file  L0_base.h
 *  \brief Prototypes of the L0Base library.
 *  \version SEcube Open Source SDK 1.5.1
 */

#ifndef _L0_BASE_H
#define _L0_BASE_H

#include <stdio.h>
#include <memory>
#include <vector>
#include "../L0_enumerations.h"

#ifdef _WIN32
	#include "Windows.h"
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <string.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <malloc.h>
	#include <errno.h>
#endif

#define SE3_DRIVE_BUF_MAX 1024
#define SE3_MAGIC_FILE_LEN 9
#define SE3C_MAGIC_TIMEOUT 1000

#define SE3GET16(x, pos, val) do{ memcpy((void*)&(val), ((uint8_t*)(x))+pos, 2); }while(0)
#define SE3GET32(x, pos, val) do{ memcpy((void*)&(val), ((uint8_t*)(x))+pos, 4); }while(0)
#define SE3SET16(x, pos, val) do{ memcpy(((uint8_t*)(x))+pos, (void*)&(val), 2); }while(0)
#define SE3SET32(x, pos, val) do{ memcpy(((uint8_t*)(x))+pos, (void*)&(val), 4); }while(0)

#ifdef _WIN32
	#define SE3_OSSEP L'\\\\' /* error #define SE3_OSSEP L'\\' -> only one backslash */
	#define SE3_MAGIC_FILE L".se3magic"
	#define Se3Sleep() Sleep(0)
#else
	#define SE3_OSSEP '/' /* error #define SE3_OSSEP L'\' -> nop backshash miss ' */
	#define SE3_MAGIC_FILE ".se3magic"
	#define Se3Sleep() usleep(1000)
#endif

#ifdef _DEBUG
	#define Se3Trace(msg) printf msg
#else
	#define Se3Trace(msg)
#endif

const uint8_t se3Magic[L0Communication::Size::MAGIC] = {
    0x3c, 0xab, 0x78, 0xb6, 0x2, 0x64, 0x47, 0xe9, 0x30, 0x26, 0xd4, 0x1f, 0xad, 0x68, 0x22, 0x27,
    0x41, 0xa4, 0x32, 0xba, 0xbe, 0x54, 0x83, 0xee, 0xab, 0x6b, 0x62, 0xce, 0xf0, 0x5c, 0x7, 0x91
};

const uint16_t se3Crc16Table[0x100] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 };

#ifdef _WIN32
	typedef wchar_t se3Char;
#else
	typedef char se3Char;
#endif

#ifdef _WIN32
	typedef BOOL(WINAPI *tADVAPI32_CryptAcquireContextW)(
		ULONG_PTR  *phProv,
		LPCWSTR    szContainer,
		LPCWSTR    szProvider,
		DWORD       dwProvType,
		DWORD       dwFlags);

	typedef BOOL(WINAPI *tADVAPI32_CryptGenRandom)(
		ULONG_PTR  hProv,
		DWORD   dwLen,
		BYTE    *pbBuffer);

	typedef BOOL(WINAPI *tADVAPI32_CryptReleaseContext)(
		ULONG_PTR  hProv,
		DWORD       dwFlags);
#endif

typedef struct se3DiscoverInfo_ {
	uint8_t serialno[L0Communication::Size::SERIAL];
	uint8_t hello_msg[L0Communication::Size::HELLO];
	uint16_t status;
} se3DiscoverInfo;

typedef struct se3DeviceInfo_ {
	se3Char path[L0Communication::Parameter::SE3_MAX_PATH];
    uint8_t serialno[L0Communication::Size::SERIAL];
    uint8_t helloMsg[L0Communication::Size::HELLO];
    uint16_t status;
} se3DeviceInfo;

typedef struct se3DriveIt_ {
	se3Char* path;
	se3Char buf[SE3_DRIVE_BUF_MAX + 1];
	size_t bufLen;
#ifdef _WIN32
	size_t pos;
#else
	FILE* fp;
#endif
} se3DriveIt;

typedef struct se3DiscoIt_ {
	se3DeviceInfo deviceInfo;
	se3DriveIt driveIt;
} se3DiscoIt;

typedef struct {
#ifdef _WIN32
	OVERLAPPED ol;
	HANDLE h;
#else
	int fd;
	void* buf;
	bool locked;
#endif
}se3File;

typedef struct se3Device_ {
	se3DeviceInfo info;
	std::shared_ptr<uint8_t> request;
	std::shared_ptr<uint8_t> response;
	se3File f;
	bool opened;
} se3Device;

class L0Base {
	private:
		se3DiscoIt it;
		std::vector<se3Device> dev;
		uint8_t ptr;
	public:
		L0Base();
		~L0Base();
		void AddDevice(); /**< Add a device to the array. */
		size_t GetNDevices(); /**< Get the number of connected devices. */
		void ResetDeviceArray(); /**< Clear the content of the device array. */
		//Device GET methods
		se3File		GetDeviceFile();
		uint8_t*	GetDeviceHelloMsg();
		se3Char*	GetDeviceInfoPath();
		uint8_t*	GetDeviceInfoSerialNo();
		bool		GetDeviceOpened();
		uint8_t		GetDevicePtr();
		uint8_t*	GetDeviceRequest();
		uint8_t*	GetDeviceResponse();
		//Iterator GET methods
		uint8_t*	GetDiscoDeviceHelloMsg();
		se3Char*	GetDiscoDevicePath();
		uint8_t*	GetDiscoDeviceSerialNo();
		se3Char*	GetDiscoDriveBuf();
		size_t		GetDiscoDriveBufLen();
		se3Char*	GetDiscoDrivePath();
		FILE * 		GetDiscoDriveFile();
		//buffer allocation/deallocation
		void	AllocateDeviceRequest();
		void	AllocateDeviceResponse();
		void	FreeDeviceRequest();
		void	FreeDeviceResponse();
		//device SET methods
		void	SetDeviceFile(se3File file);
		void	SetDeviceOpened(bool opened);
		bool	SetDevicePtr(uint16_t newPtr);
		//iterator SET methods
		void	SetDiscoDeviceStatus(uint16_t status);
		void	SetDiscoDriveBufLen(size_t bufLen);
		void	SetDiscoDriveBufTermination();
		void	SetDiscoDrivePath(se3Char* path);
		void 	SetDiscoDriveFile(FILE* fp);
};

class L0Support {
	private:
		L0Support() {};
	public:
		static void Se3PathCopy(se3Char* dest, se3Char* src);
		static bool Se3Win32DiskInDrive(wchar_t* path);
		static uint64_t Se3Deadline(uint32_t timeout);
		static uint64_t Se3Clock();
		static bool Se3WriteMagic(se3File hFile);
		static bool Se3Write(uint8_t* buf, se3File hfile, size_t block, size_t nBlocks, uint32_t timeout);
		static bool Se3Read(uint8_t* buf, se3File hFile, size_t block, size_t nBlocks, uint32_t timeout);
		static void Se3Close(se3File hFile);
		static uint16_t Se3OpenExisting(se3Char* path, bool rw, uint64_t deadline, se3File* phFile);
		static void Se3MakePath(se3Char* dest, se3Char* src);
		static bool Se3MagicInit(se3Char* path, uint8_t* discoBuf, se3DiscoverInfo* info);
		static bool Se3ReadInfo(uint8_t* buf, se3DiscoverInfo* info);
		static void Se3Rand(size_t len, uint8_t* buf);
		static uint16_t Se3ReqLenDataAndHeaders(uint16_t dataLen);
		static uint16_t Se3RespLenData(uint16_t lenDataAndHeaders);
		static uint16_t Se3NBlocks(uint16_t len);
		static uint16_t Se3Crc16Update(size_t dataLen, const uint8_t* data, uint16_t crc);
		static bool se3UnixLock(int fd);
		static void se3UnixUnlock(int fd);
		static void DebugFileCreation();
};

#endif
