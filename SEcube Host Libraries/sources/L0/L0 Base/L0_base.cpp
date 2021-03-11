/**
  ******************************************************************************
  * File Name          : L0_base.cpp
  * Description        : Implementation of the L0Base library.
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

/**
 * @file	L0_base.cpp
 * @date	July, 2017
 * @brief	Implementation of the L0Base library
 *
 * The file contains all the implementation of the L0Base library
 */

#include "L0_base.h"
#include <memory>
#include <time.h>

L0Base::L0Base() {
	this->it = {};
	this->ptr = 0;
}

L0Base::~L0Base() {
	dev.clear();
}

//add a device to the array
void L0Base::AddDevice() {
	//create a temporary device
	se3Device _dev;

	//fill the device with the iterator data
	memcpy(_dev.info.serialno, this->it.deviceInfo.serialno, L0Communication::Size::SERIAL);
	memcpy(_dev.info.helloMsg, this->it.deviceInfo.helloMsg, L0Communication::Size::HELLO);
	L0Support::Se3PathCopy(_dev.info.path, this->it.deviceInfo.path);
	_dev.info.status = this->it.deviceInfo.status;
	_dev.opened = false;
	_dev.f = {0};

	//add the device to the vector
	this->dev.push_back(_dev);
}

//get the amount of devices in the array
size_t L0Base::GetNDevices() {
	return dev.size();
}

void L0Base::ResetDeviceArray() {
	this->dev.clear();
}


///////////////
//GET METHODS//
///////////////
//device GET methods
se3File L0Base::GetDeviceFile() {
	return this->dev[this->ptr].f;
}

uint8_t* L0Base::GetDeviceHelloMsg() {
	return this->dev[this->ptr].info.helloMsg;
}

se3Char* L0Base::GetDeviceInfoPath() {
	return this->dev[this->ptr].info.path;
}

uint8_t* L0Base::GetDeviceInfoSerialNo() {
	return this->dev[this->ptr].info.serialno;
}

bool L0Base::GetDeviceOpened() {
	return this->dev[this->ptr].opened;
}

uint8_t L0Base::GetDevicePtr() {
	return this->ptr;
}

uint8_t* L0Base::GetDeviceRequest() {
	return this->dev[this->ptr].request.get();
}

uint8_t* L0Base::GetDeviceResponse() {
	return this->dev[this->ptr].response.get();
}

//******************************//
//Iterator GET methods
uint8_t*	L0Base::GetDiscoDeviceHelloMsg() {
	return this->it.deviceInfo.helloMsg;
}

se3Char*	L0Base::GetDiscoDevicePath() {
	return this->it.deviceInfo.path;
}

uint8_t*	L0Base::GetDiscoDeviceSerialNo() {
	return this->it.deviceInfo.serialno;
}

se3Char*	L0Base::GetDiscoDriveBuf() {
	return this->it.driveIt.buf;
}

size_t		L0Base::GetDiscoDriveBufLen() {
	return this->it.driveIt.bufLen;
}

se3Char*	L0Base::GetDiscoDrivePath() {
	return this->it.driveIt.path;
}


///////////////
//SET METHODS//
///////////////
//buffer allocation/deallocation
void L0Base::AllocateDeviceRequest() {
	std::shared_ptr<uint8_t> sp(new uint8_t[L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK], std::default_delete<uint8_t[]>());
	memset(sp.get(), 0, L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK);
	this->dev[this->ptr].request = std::move(sp);
}

//method to allocate the memory for the response buffer
void L0Base::AllocateDeviceResponse() {
	std::shared_ptr<uint8_t> sp(new uint8_t[L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK], std::default_delete<uint8_t[]>());
	memset(sp.get(), 0, L0Communication::Parameter::COMM_N * L0Communication::Parameter::COMM_BLOCK);
	this->dev[this->ptr].response = std::move(sp);
}

void L0Base::FreeDeviceRequest() {
	this->dev[this->ptr].request.reset();
	this->dev[this->ptr].request = nullptr;
}

void L0Base::FreeDeviceResponse() {
	this->dev[this->ptr].response.reset();
	this->dev[this->ptr].response = nullptr;
}


//device SET methods
void L0Base::SetDeviceFile(se3File file) {
	this->dev[this->ptr].f = file;
}

void L0Base::SetDeviceOpened(bool opened) {
	this->dev[this->ptr].opened = opened;
}

//change the device ptr
bool L0Base::SetDevicePtr(uint16_t newPtr) {
	//check if there is no device in the vector or if pointing outside the vector
	if (this->dev.size() == 0 || newPtr > UINT8_MAX || newPtr > this->dev.size() - 1) {
		return false;
	}

	this->ptr = newPtr;
	return true;
}

//iterator SET methods
void L0Base::SetDiscoDriveBufLen(size_t bufLen) {
	this->it.driveIt.bufLen = bufLen;
}

void L0Base::SetDiscoDriveBufTermination() {
	this->it.driveIt.buf[this->it.driveIt.bufLen] = L'\0';
}

#ifdef _WIN32
//WINDOWS
void L0Base::SetDiscoDrivePath(se3Char* path) {
	this->it.driveIt.path = path;
}
#else
//UNIX
void L0Base::SetDiscoDrivePath(se3Char* path){
	this->it.driveIt.path = path;
}
void L0Base::SetDiscoDriveFile(FILE* fp) {
	this->it.driveIt.fp = fp;
}

FILE* L0Base::GetDiscoDriveFile() {
	return this->it.driveIt.fp;
}
#endif

void L0Base::SetDiscoDeviceStatus(uint16_t status) {
	this->it.deviceInfo.status = status;
}

///////////////////////////////////
//L0 Support class implementation//
///////////////////////////////////

void L0Support::Se3PathCopy(se3Char* dest, se3Char* src) {
#ifdef _WIN32
	wcscpy(dest, src);
#else
	strcpy(dest, src);
#endif
}

#ifdef _WIN32
bool L0Support::Se3Win32DiskInDrive(wchar_t* path) {
	/*
	Source: https://social.msdn.microsoft.com/Forums/vstudio/en-US/18ef7c0b-2dab-40e6-aae8-9fbf55cb8686/how-to-detect-if-there-is-a-disk-in-the-drive?forum=vcgeneral
	 */
	wchar_t volume[MAX_PATH];
	DWORD bytesReturned; // ignored
	BOOL devSuccess;
	HANDLE h;
	DWORD lastError;

	wcscpy(volume, L"\\\\.\\");
	wcsncat(volume, path, 2);

	h = CreateFileW(	volume,
						GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);

	if (h == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		return (lastError != 2) ? (true) : (false);
	}

	devSuccess = DeviceIoControl(h, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &bytesReturned, NULL);

	if (devSuccess == FALSE) {
		lastError = GetLastError();
		CloseHandle(h);
		return false;
	}

	CloseHandle(h);
	return true;
}
#endif

uint64_t L0Support::Se3Deadline(uint32_t timeout) {
	return Se3Clock() + timeout;
}

uint64_t L0Support::Se3Clock() {
#ifdef _WIN32
//WINDOWS
	uint64_t ms = (uint64_t)clock();
	ms = (ms * 1000) / CLOCKS_PER_SEC;
	return ms;
#else
//UNIX
	uint64_t ms;
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	ms = spec.tv_sec;
	ms *= 1000;
	ms += (uint64_t)spec.tv_nsec / ((uint64_t)1000000);
	return ms;
#endif
}

bool L0Support::Se3WriteMagic(se3File hFile) {
    size_t i;
    //MAGIC = 32
    //COMM = 512
    uint8_t buf[L0Communication::Parameter::COMM_BLOCK];
    //16 loop
    for (i = 0; i < L0Communication::Parameter::COMM_BLOCK; i += L0Communication::Size::MAGIC)
        memcpy(buf + i, se3Magic, L0Communication::Size::MAGIC);
    for (i = 0; i < 16; i++) {
        buf[L0Communication::Parameter::COMM_BLOCK - 1] = (uint8_t)i;
        if (!Se3Write(buf, hFile, i, 1, SE3C_MAGIC_TIMEOUT))
            return false;
    }
    return true;
}

#ifdef _WIN32
//WINDOWS
bool L0Support::Se3Write(uint8_t* buf, se3File hfile, size_t block, size_t nBlocks, uint32_t timeout) {
	bool success = false;
	DWORD bytes_written = 0;
	DWORD wret, e;
	hfile.ol.Offset = (DWORD)(block * L0Communication::Parameter::COMM_BLOCK);
	hfile.ol.OffsetHigh = 0;
	if (!WriteFile(hfile.h, buf, (DWORD)(nBlocks * L0Communication::Parameter::COMM_BLOCK), &bytes_written, &hfile.ol)) {
		e = GetLastError();
		if (e == ERROR_IO_PENDING) {
			wret = WaitForSingleObject(hfile.ol.hEvent, timeout);
			if (WAIT_OBJECT_0 == wret) {
				if (GetOverlappedResult(hfile.h, &hfile.ol, &bytes_written, FALSE)) {
					if (bytes_written == nBlocks * L0Communication::Parameter::COMM_BLOCK)
						success = true;
				}
			}
			else if (WAIT_TIMEOUT == wret)
				Se3Trace(("se3c_write timeout\n"));
		}
	}
	else
		success = true;
	if (!success)
		Se3Trace(("se3c_write fail\n"));
	return success;
}
#else
//UNIX
bool L0Support::Se3Write(uint8_t* buf, se3File hfile, size_t block, size_t nBlocks, uint32_t timeout) {
    memcpy(hfile.buf, buf, nBlocks * L0Communication::Parameter::COMM_BLOCK);
    if (nBlocks * L0Communication::Parameter::COMM_BLOCK != pwrite(	hfile.fd,
    																hfile.buf,
																	nBlocks * L0Communication::Parameter::COMM_BLOCK,
																	block * L0Communication::Parameter::COMM_BLOCK)) {
        return false;
    }
    return true;
}
#endif

#ifdef _WIN32
//WINDOWS
bool L0Support::Se3Read(uint8_t* buf, se3File hFile, size_t block, size_t nBlocks, uint32_t timeout) {
	bool success = false;
	DWORD bytesRead = 0;
	DWORD wRet, e;
	hFile.ol.Offset = (DWORD)(block * L0Communication::Parameter::COMM_BLOCK);
	hFile.ol.OffsetHigh = 0;
	if (!ReadFile(hFile.h, buf, (DWORD)(nBlocks * L0Communication::Parameter::COMM_BLOCK), NULL, &hFile.ol)) {
		e = GetLastError();
		if (e == ERROR_IO_PENDING) {
			wRet = WaitForSingleObject(hFile.ol.hEvent, timeout);
			if (WAIT_OBJECT_0 == wRet) {
				if (GetOverlappedResult(hFile.h, &hFile.ol, &bytesRead, FALSE)) {
					if (bytesRead == nBlocks * L0Communication::Parameter::COMM_BLOCK)
						success = true;
				}
				else
					e = GetLastError();
			}
			else if (wRet == WAIT_TIMEOUT)
				Se3Trace(("se3c_read timeout\n"));
		}
	}
	else
		success = true;
	if (!success)
		Se3Trace(("se3c_read fail\n"));

	return success;
}
#else
//UNIX
bool L0Support::Se3Read(uint8_t* buf, se3File hFile, size_t block, size_t nBlocks, uint32_t timeout) {
    if (nBlocks * L0Communication::Parameter::COMM_BLOCK != pread(	hFile.fd,
    																hFile.buf,
																	nBlocks * L0Communication::Parameter::COMM_BLOCK,
																	block * L0Communication::Parameter::COMM_BLOCK))
    {
    	return false;
    }
    memcpy(buf, hFile.buf, nBlocks * L0Communication::Parameter::COMM_BLOCK);
    return true;
}
#endif

#ifdef _WIN32
//WINDOWS
void L0Support::Se3Close(se3File hFile) {
	if (hFile.h != INVALID_HANDLE_VALUE && hFile.h != 0) {
		CloseHandle(hFile.ol.hEvent);
		CloseHandle(hFile.h);
		hFile.h = INVALID_HANDLE_VALUE;
	}
}
#else
//UNIX
void L0Support::Se3Close(se3File hFile) {
    if (hFile.fd >= 0) {
    	se3UnixUnlock(hFile.fd);
    	hFile.locked = false;
    }
	if (hFile.fd >= 0) {
        close(hFile.fd);
        hFile.fd = -1;
    }
    if (hFile.buf != NULL) {
    	free(hFile.buf);
        hFile.buf = NULL;
    }
}
#endif
#ifdef _WIN32
uint16_t L0Support::Se3OpenExisting(se3Char* path, bool rw, uint64_t deadline, se3File* phFile) {

	uint16_t ret = L0Communication::Error::OK;
	HANDLE h = INVALID_HANDLE_VALUE;
	DWORD e;
	se3Char mfPath[L0Communication::Parameter::SE3_MAX_PATH];
	Se3MakePath(mfPath, path);
	Se3Trace(("se3c_open_existing %ls\n", mfPath));

	do {
		h = CreateFileW(	mfPath,
							(rw) ? (GENERIC_READ | GENERIC_WRITE) : (GENERIC_READ),
							(rw) ? (FILE_SHARE_READ) : (FILE_SHARE_READ | FILE_SHARE_WRITE),
							NULL,
							OPEN_EXISTING,
							(rw) ? (FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED) : (FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED),
							NULL);
		if (h != INVALID_HANDLE_VALUE)
			break;
		e = GetLastError();

		if (e == ERROR_FILE_NOT_FOUND) {
			ret = L0Communication::Error::ERR_NOT_FOUND;
			break;
		}
		if (e != ERROR_FILE_CHECKED_OUT)
			return L0Communication::Error::ERR_NO_DEVICE;

		Se3Sleep();

		if (Se3Clock() > deadline && h == INVALID_HANDLE_VALUE) {
			ret = L0Communication::Error::ERR_TIMEOUT;
			break;
		}
	} while(h == INVALID_HANDLE_VALUE);

	if (ret == L0Communication::Error::OK) {
		phFile->h = h;
		memset((void*)&(phFile->ol), 0, sizeof(OVERLAPPED));
		phFile->ol.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	}
	else
		phFile->h = INVALID_HANDLE_VALUE;

	return ret;
}
#else
//UNIX
uint16_t L0Support::Se3OpenExisting(se3Char* path, bool rw, uint64_t deadline, se3File* phFile) {
	uint16_t ret = L0Communication::Error::OK;
	int fd = -1; 	//set the file descriptor to an invalid value
	bool lock_success = false;
	se3Char mfPath[L0Communication::Parameter::SE3_MAX_PATH];
	Se3MakePath(mfPath, path);
//	Se3Trace(("se3c_open_existing %ls\n", mfPath));
	phFile->locked = false;
	if (rw)
		fd = open (mfPath, O_RDWR | O_DIRECT | O_SYNC, S_IWUSR | S_IRUSR);
	else
		fd = open (mfPath, O_RDONLY | O_DIRECT | O_SYNC, S_IWUSR | S_IRUSR);
	if (fd<0) {
	        if (errno == ENOENT) {
	        	ret = L0Communication::Error::ERR_NOT_FOUND;
	        }
	        else {
	        	return L0Communication::Error::ERR_NO_DEVICE;
	        }
	 }
	if (rw) {
	        do
	        {
	            lock_success = se3UnixLock(fd);
	            if (lock_success) {
	                ret = L0Communication::Error::OK;
	                break;
	            }
	            else {
	                printf("lock fail %s\n", mfPath);
	            }

	            Se3Sleep();
	            if (Se3Clock() > deadline) {
	            	ret = L0Communication::Error::ERR_TIMEOUT;
	                break;
	            }
	        } while (!lock_success);
	    }

	if (ret == L0Communication::Error::OK) {
		phFile->fd = fd;
		phFile->locked = true;
		phFile->buf = memalign(L0Communication::Parameter::COMM_BLOCK,L0Communication::Parameter::COMM_BLOCK * 16);
	} else {
		phFile->fd = -1;
	}
	return ret;
}
#endif

void L0Support::Se3MakePath(se3Char* dest, se3Char* src) {
#ifdef _WIN32
	size_t len = 0;
	std::wstring dest_;
	std::wstring m = L".se3magic"; // SE3_MAGIC_FILE
	std::wstring w = L"\\\\"; // SE3_OSSEP
	size_t se3magicfilelen = m.length();
	//while ((src[len] != (se3Char)'\0') && (len < (L0Communication::Parameter::SE3_MAX_PATH - SE3_MAGIC_FILE_LEN - 1))){
	while ((len < (L0Communication::Parameter::SE3_MAX_PATH - se3magicfilelen - 1)) && (src[len] != (se3Char)'\0')){
		dest_.push_back(src[len]); // copy wchar_t to wstring
		len++; // increment until NULL terminator or max path len reached
	}
	if (len == (L0Communication::Parameter::SE3_MAX_PATH - /*SE3_MAGIC_FILE_LEN*/ se3magicfilelen - 1)) {
		dest[0] = (se3Char)'\0'; // max path len reached without finding NULL terminator, error
		return;
	}
	//	memcpy(dest, src, len * sizeof(se3Char));
	//	if (dest[len - 1] != SE3_OSSEP){ // check definition of SE3_OSSEP for Windows (wchar_t)
	//		dest[len++] = SE3_OSSEP;
	//	}
	//	memcpy(dest + len, SE3_MAGIC_FILE, SE3_MAGIC_FILE_LEN * sizeof(se3Char));
	//	len += SE3_MAGIC_FILE_LEN;
	//	dest[len] = (se3Char)'\0';
	if (dest_.length() < w.length()) {
		dest[0] = (se3Char)'\0';
		return;
	}
	if (dest_.compare(dest_.length() - w.length(), w.length(), w) != 0){ // add double backslash if required
		dest_.append(w);
	}
	dest_.append(m); // append SE3_MAGIC_FILE
	size_t i=0;
	for(wchar_t wct : dest_){ // copy wstring to primitive wchar_t array
		dest[i++] = wct;
	}
	dest[i] = L'\0'; // append null terminator
#else // UNIX
	size_t len = 0;
	while ((len < (L0Communication::Parameter::SE3_MAX_PATH - SE3_MAGIC_FILE_LEN - 1)) && (src[len] != (se3Char)'\0')){
		len++;
	}
	if (len == (L0Communication::Parameter::SE3_MAX_PATH - SE3_MAGIC_FILE_LEN - 1)) {
		dest[0] = (se3Char)'\0';
		return;
	}
	memcpy(dest, src, len * sizeof(se3Char));
	if (dest[len - 1] != SE3_OSSEP){
		dest[len++] = SE3_OSSEP;
	}
	memcpy(dest + len, SE3_MAGIC_FILE, SE3_MAGIC_FILE_LEN * sizeof(se3Char));
	len += SE3_MAGIC_FILE_LEN;
	dest[len] = (se3Char)'\0';
#endif
}

#ifdef _WIN32
//WINDOWS
bool L0Support::Se3MagicInit(se3Char* path, uint8_t* discoBuf, se3DiscoverInfo* info) {
	se3File hFile;
	se3Char mfPath[L0Communication::Parameter::SE3_MAX_PATH];
	se3DiscoverInfo info_;
	Se3MakePath(mfPath, path);
	// eclusive open r/w, create if not exists
	hFile.h = CreateFileW(	mfPath,
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_ALWAYS,
							FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
							0);
	Se3Trace(("se3c_magic_init %ls\n", mfPath));
	if (hFile.h == INVALID_HANDLE_VALUE)	// cannot open
		return false;

	memset((void*)&(hFile.ol), 0, sizeof(OVERLAPPED));
	hFile.ol.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

	if (!Se3WriteMagic(hFile)) {			// cannot write
		Se3Close(hFile);
		DeleteFileW(mfPath);
		return false;
	}

	if (!Se3Read(discoBuf, hFile, 15, 1, SE3C_MAGIC_TIMEOUT)) {	// cannot read
		Se3Close(hFile);
		DeleteFileW(mfPath);
		return false;
	}

	if (!Se3ReadInfo(discoBuf, &info_)) {	// not a SECube
		Se3Close(hFile);
		DeleteFileW(mfPath);
		return false;
	}

	if (info != NULL) {
		memcpy(info, &info_, sizeof(se3DiscoverInfo));
	}

	Se3Close(hFile);
	return true;
}
#else
//UNIX
bool L0Support::Se3MagicInit(se3Char* path, uint8_t* discoBuf, se3DiscoverInfo* info) {
    se3File hFile;
    se3Char mfPath[L0Communication::Parameter::SE3_MAX_PATH];
    se3DiscoverInfo info_;
    Se3MakePath(mfPath, path);
    // eclusive open r/w, create if not exists

    hFile.locked = false;
    hFile.fd = open((char*)mfPath, O_SYNC | O_RDWR | O_CREAT | O_DIRECT | O_TRUNC, S_IWUSR | S_IRUSR);

    Se3Trace(("se3c_magic_init %s\n", mfPath));
    if (hFile.fd < 0) {
        // cannot open
        return false;
    }

    hFile.buf = memalign(L0Communication::Parameter::COMM_BLOCK, L0Communication::Parameter::COMM_BLOCK);
    if (hFile.buf == NULL) {
        // memalign failed
        close(hFile.fd);
        unlink(mfPath);
        return false;
    }

    if (! se3UnixLock(hFile.fd)) {
        // cannot lock
        close(hFile.fd);
        return false;
    }
    hFile.locked = true;

    if (!Se3WriteMagic(hFile)) {
        // cannot write
        Se3Close(hFile);
        unlink(mfPath);
        return false;
    }
    if (!Se3Read(discoBuf, hFile, 15, 1, SE3C_MAGIC_TIMEOUT)) {
        // cannot read
        Se3Close(hFile);
        unlink(mfPath);
        return false;
    }

    if (!Se3ReadInfo(discoBuf, &info_)) {
        // not a SECube
        Se3Close(hFile);
        unlink(mfPath);
        return false;
    }

    if (info != NULL) {
        memcpy(info, &info_, sizeof(se3DiscoverInfo));
    }

    Se3Close(hFile);
    return true;
}
#endif

bool L0Support::Se3ReadInfo(uint8_t* buf, se3DiscoverInfo* info) {
	uint8_t magicInv[L0Communication::Size::MAGIC];

	// Magic number is reversed on file; Swap High with Low and store in magic_inv
	memcpy(magicInv + L0Communication::Size::MAGIC / 2, buf, L0Communication::Size::MAGIC / 2);
	memcpy(magicInv, buf + L0Communication::Size::MAGIC / 2, L0Communication::Size::MAGIC / 2);

	if (memcmp(magicInv, se3Magic, L0Communication::Size::MAGIC))
		return false;

	if (info != NULL) {
		memcpy(info->serialno, buf + L0DiscoverParameters::Offset::SERIAL, L0Communication::Size::SERIAL);		//serial number
		//copy the hello message from the buffer to the info struct
		memcpy(info->	hello_msg,										//destination
						buf + L0DiscoverParameters::Offset::HELLO,		//source
						L0Communication::Size::HELLO);					//size
		SE3GET16(buf, L0DiscoverParameters::Offset::STATUS, info->status);					//device status
	}

	return true;
}

void L0Support::Se3Rand(size_t len, uint8_t* buf) {
//maybe change this
#ifdef _WIN32
//WINDOWS
	static HMODULE hAdvapi32 = NULL;
	static tADVAPI32_CryptAcquireContextW ADVAPI32_CryptAcquireContextW = NULL;
	static tADVAPI32_CryptGenRandom ADVAPI32_CryptGenRandom = NULL;
	static tADVAPI32_CryptReleaseContext ADVAPI32_CryptReleaseContext = NULL;
	static bool wincrypt_available = false;
	ULONG_PTR hProvider = 0;

	if(!hAdvapi32) {
		hAdvapi32 = LoadLibraryW(L"Advapi32.dll");
		if (hAdvapi32 != NULL) {
			ADVAPI32_CryptAcquireContextW = (tADVAPI32_CryptAcquireContextW)GetProcAddress(hAdvapi32, "CryptAcquireContextW");
			ADVAPI32_CryptGenRandom = (tADVAPI32_CryptGenRandom)GetProcAddress(hAdvapi32, "CryptGenRandom");
			ADVAPI32_CryptReleaseContext = (tADVAPI32_CryptReleaseContext)GetProcAddress(hAdvapi32, "CryptReleaseContext");
			if (ADVAPI32_CryptAcquireContextW != NULL &&  ADVAPI32_CryptGenRandom != NULL && ADVAPI32_CryptReleaseContext != NULL)
				wincrypt_available = true;
		}
	}

	if (wincrypt_available)
		if (FALSE == ADVAPI32_CryptAcquireContextW(&hProvider, NULL, NULL, L0Win32ApiCodes::Codes::FULL, 0))
			if (GetLastError() == (unsigned)L0Win32ApiCodes::Codes::BAD_KEYSET)
				if (FALSE == ADVAPI32_CryptAcquireContextW(&hProvider, NULL, NULL, L0Win32ApiCodes::Codes::FULL, L0Win32ApiCodes::Codes::NEW_KEYSET))
					hProvider = 0;

	if (hProvider != 0) {
		ADVAPI32_CryptGenRandom(hProvider, (DWORD)len, buf);
		ADVAPI32_CryptReleaseContext(hProvider, 0U);
	}

	else {
		for (size_t i = 0; i < len; i++)
			buf[i] = (uint8_t)rand();
	}
#else
//UNIX
	int frnd = open("/dev/urandom", O_RDONLY);
	read(frnd, buf, len);
	close(frnd);
#endif
}

uint16_t L0Support::Se3ReqLenDataAndHeaders(uint16_t dataLen) {
	uint16_t nDataBlocks;

	if (dataLen <= L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER)
		return dataLen + L0Request::Size::HEADER;

	dataLen -= L0Communication::Parameter::COMM_BLOCK - L0Request::Size::HEADER;
	nDataBlocks = dataLen / (L0Communication::Parameter::COMM_BLOCK - L0Request::Size::DATA_HEADER);

	if (dataLen % (L0Communication::Parameter::COMM_BLOCK - L0Request::Size::DATA_HEADER) != 0)
		nDataBlocks++;

	return L0Communication::Parameter::COMM_BLOCK + dataLen + nDataBlocks * L0Request::Size::DATA_HEADER;
}

uint16_t L0Support::Se3RespLenData(uint16_t lenDataAndHeaders) {
	uint16_t nBlocks;

	if (lenDataAndHeaders < L0Response::Size::HEADER)
		return 0;

	nBlocks = lenDataAndHeaders / L0Communication::Parameter::COMM_BLOCK;

	if (lenDataAndHeaders % L0Communication::Parameter::COMM_BLOCK != 0)
		nBlocks++;

	if (nBlocks == 0)
		return 0;

	return lenDataAndHeaders - L0Response::Size::HEADER - (nBlocks - 1) * L0Response::Size::DATA_HEADER;
}

uint16_t L0Support::Se3NBlocks(uint16_t len) {
	uint16_t nBlocks = len / L0Communication::Parameter::COMM_BLOCK;
	if (len % L0Communication::Parameter::COMM_BLOCK != 0)
		nBlocks++;

	return nBlocks;
}

uint16_t L0Support::Se3Crc16Update(size_t dataLen, const uint8_t* data, uint16_t crc) {
	while (dataLen--) {
		crc = (crc << 8) ^ se3Crc16Table[(crc >> 8) ^ *data];
		data++;
	}

	return crc;
}

#ifndef _WIN32
/* UNIX file operations */
bool L0Support::se3UnixLock(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();
    return (-1 != fcntl(fd, F_SETLK, &fl));
}
void L0Support::se3UnixUnlock(int fd) {
    struct flock fl;
    fl.l_type = F_ULOCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();
    fcntl(fd, F_SETLK, &fl);
}
#endif
