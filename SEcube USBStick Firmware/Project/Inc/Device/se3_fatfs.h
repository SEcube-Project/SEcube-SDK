#ifndef SRC_DEVICE_SE3_FATFS_H_
#define SRC_DEVICE_SE3_FATFS_H_

#include "se3_security_core.h"
#include "fatfs.h"

#define SE3_FATFS_KEY_SIZE 16

#define SEFILE_NONCE_LEN 32
#define SIGNATURE_LEN 32
#define SEFILE_HEADER_PLAINTXT_LEN 16
#define SE3_FILE_SECTOR_SIZE _MAX_SS
#define SE3_SECTOR_ENCRYPTED_DATA_SIZE (SE3_FILE_SECTOR_SIZE - SIGNATURE_LEN)
#define SEFILE_IV_LEN 16
#define SEFILE_LOGIC_DATA (SE3_FILE_SECTOR_SIZE - sizeof(uint16_t) - SIGNATURE_LEN)

typedef struct
{
	FIL fp;
	uint32_t keyID;
	uint16_t algo;
	uint8_t decrypt_buffer[SEFILE_LOGIC_DATA];
	uint16_t decrypt_buffer_size;
	uint8_t mode;
	bool dirty_bit;
	uint32_t pointer;
	uint8_t IV[SEFILE_IV_LEN];
} SE3_FIL;


#define MAX_PATHNAME 512

//Copia e incolla da fatfs, da estendere con errori relativi alla parte crittografica
typedef enum {
	SE3_FR_OK = 0,				/* (0) Succeeded */
	SE3_FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	SE3_FR_INT_ERR,				/* (2) Assertion failed */
	SE3_FR_NOT_READY,			/* (3) The physical drive cannot work */
	SE3_FR_NO_FILE,				/* (4) Could not find the file */
	SE3_FR_NO_PATH,				/* (5) Could not find the path */
	SE3_FR_INVALID_NAME,		/* (6) The path name format is invalid */
	SE3_FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	SE3_FR_EXIST,				/* (8) Access denied due to prohibited access */
	SE3_FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	SE3_FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	SE3_FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	SE3_FR_NOT_ENABLED,			/* (12) The volume has no work area */
	SE3_FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	SE3_FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	SE3_FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	SE3_FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	SE3_FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	SE3_FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	SE3_FR_INVALID_PARAMETER,	/* (19) Given parameter is invalid */
	SE3_FR_NO_KEY,
	SE3_FR_INVALID_ALGO,
	SE3_FR_FILENAME_ENC_ERROR,
	SE3_FR_HEADER_ENC_ERROR,
	SE3_FR_DATA_ENC_ERROR,
	SE3_FR_CYPHER_ERROR,
	SE3_FR_INVALID_SIGNATURE
} SE3_FRESULT;

SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, BYTE mode, uint32_t keyID, uint16_t algo);
SE3_FRESULT secure_close(SE3_FIL* fp);
SE3_FRESULT secure_read(SE3_FIL* fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead);
SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len);
SE3_FRESULT secure_seek(SE3_FIL* fp, int32_t offset, int32_t *position, uint8_t whence);


#endif /* SRC_DEVICE_SE3_FATFS_H_ */
