#ifndef SRC_DEVICE_SE3_FATFS_H_
#define SRC_DEVICE_SE3_FATFS_H_

#include "se3_security_core.h"
#include "fatfs.h"

typedef struct
{
	FIL fp;
	uint32_t keyID;
	uint16_t algo;
	uint8_t decrypt_buffer[_MAX_SS];
} SE3_FIL;


//Copia e incolla da fatfs, da estendere con errori relativi alla parte crittografica
typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} SE3_FRESULT;

SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo);
SE3_FRESULT secure_close(SE3_FIL* fp);
SE3_FRESULT secure_read(SE3_FIL* fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead);
SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len);
SE3_FRESULT secure_seek(SE3_FIL* fp, int32_t offset, int32_t *position, uint8_t whence);


#endif /* SRC_DEVICE_SE3_FATFS_H_ */
