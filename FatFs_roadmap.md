# FatFs Roadmap

## Done
Integrate FatFs driver into SECube firmware

## TODO

Design a secure file access library implementing the following basic functions (SE3_RESULT is an enum used to identify the various error/success codes that function could return):

* SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo)

    Open a file to be later used for reading or writing data, according to the mode parameter. Since we are programming an embedded firmware and we don't have the powerful dynamic allocated C++ classes, static allocation may be used, specifying at compile time the maximum number of open files.

* SE3_FRESULT secure_close(SE3_FIL* fp)
    
    Free the resources occupied by an open file.

* SE3_FRESULT secure_read(SE3_FIL* fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead)

    The number of bytes requested as clear text is provided in uint32_t dataOut_len while the
actual number of read bytes is stored in bytesRead.

* SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len)
* SE3_FRESULT secure_seek(SE3_FIL* fp, int32_t offset, int32_t *position, uint8_t whence)

* uint16_t secure_mkdir(char* path, uint32_t keyID)
    
    It creates a directory with encrypted file name. The first 8 characters are the hexadecimal representation in ASCII of the key ID, the rest is obtained computing the AES-256-ECB of the name specified as clear text (no random padding for short names?).

secure_open_dir / close_dir / read_dir may also be implemented.

