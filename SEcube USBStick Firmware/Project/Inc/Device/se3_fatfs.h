#ifndef SRC_DEVICE_SE3_FATFS_H_
#define SRC_DEVICE_SE3_FATFS_H_

#include "se3_security_core.h"
#include "fatfs.h"

typedef struct
{
	FIL fp;
	se3_flash_key key;
	uint16_t algo;
	uint8_t decrypt_buffer[_MAX_SS];
} SE3_FIL;


//Copia e incolla da fatfs, da estendere con errori relativi alla parte crittografica
typedef enum {
	//SE3_FATFS specific
	FR_NO_KEY = 20,
	FR_INVALID_ALGO
} SE3_FRESULT;

SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo);
SE3_FRESULT secure_close(SE3_FIL* fp);
SE3_FRESULT secure_read(SE3_FIL* fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead);
SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len);
SE3_FRESULT secure_seek(SE3_FIL* fp, int32_t offset, int32_t *position, uint8_t whence);


#endif /* SRC_DEVICE_SE3_FATFS_H_ */
