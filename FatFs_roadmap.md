# FatFs Roadmap

## Done
Integrate FatFs driver into SECube firmware

## TODO

Design a secure file access library implementing the following basic functions:

* SE_FILE secure_open(char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo)

    Open a file to be later used for reading or writing data, according to the mode parameter. Since we are programming an embedded firmware and we don't have the powerful dynamic allocated C++ classes, static allocation may be used, specifing at compile time the maximum number of open files.

* uint16_t secure_close(SE_FILE fp)
    
    Free the resources occupied by an open file.

* uint16_t secure_read(SE_FILE fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead)

    The number of bytes requested as clear text is provided in uint32_t dataOut_len while the
actual number of read bytes is stored in bytesRead.

* uint16_t secure_write(SE_FILE fp, uint8_t *dataIn, uint32_t dataIn_len)
* uint16_t secure_seek(SE_FILE fp, int32_t offset, int32_t *position, uint8_t whence)

* uint16_t secure_mkdir(char* path, uint32_t keyID)
    
    It creates a directory with encrypted file name. The first 8 characters are the hexadecimal representation in ASCII of the key ID, the rest is obtained computing the AES-256-ECB of the name specified as clear text (no random padding for short names?).

secure_open_dir / close_dir / read_dir may also be implemented.

