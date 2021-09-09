#ifndef SRC_DEVICE_SE3_FATFS_H_
#define SRC_DEVICE_SE3_FATFS_H_

#include "se3_security_core.h"
#include "fatfs.h"

#define SE3_FATFS_KEY_SIZE 16

#define SE3_FATFS_NONCE_LEN 32
#define SE3_FATFS_SIGNATURE_LEN 32
#define SE3_FATFS_HEADER_PLAINTXT_LEN 16
#define SE3_FATFS_SECTOR_SIZE _MAX_SS
#define SE3_FATFS_SECTOR_ENCRYPTED_DATA_SIZE (SE3_FATFS_SECTOR_SIZE - SE3_FATFS_SIGNATURE_LEN)
#define SE3_FATFS_IV_LEN 16
#define SE3_FATFS_LOGIC_DATA (SE3_FATFS_SECTOR_SIZE - sizeof(uint16_t) - SE3_FATFS_SIGNATURE_LEN)

/**
 * @defgroup Seek_Defines
 *  @{
 */
	/**
	 * \name Defines for seek options.
	 */
	///@{
		#define SE3_FATFS_BEGIN 0
		#define SE3_FATFS_END 1
		#define SE3_FATFS_CURRENT 2
	///@}
/** @}*/





typedef struct
{
	FIL fp;
	uint32_t keyID;
	uint16_t algo;
	uint8_t decrypt_buffer[SE3_FATFS_LOGIC_DATA];
	uint16_t decrypt_buffer_size;
	uint8_t mode;
	bool dirty_bit;
	uint32_t pointer;
	uint8_t IV[SE3_FATFS_IV_LEN];
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
	SE3_FR_INVALID_SIGNATURE,
	SE3_FR_SEEK_ERROR,
	SE3_FR_NOT_IMPLEMENTED
} SE3_FRESULT;

/** @brief This function opens or creates a secure file.
 * @param [out] se_fp Pointer to a blank file object structure.
 * @param [in] path The path of a file as plaintext, terminated by a null character.
 * @param [in] mode Used to specify read-only or read-write privilege, and also to specify if we want to create a new file.
 * @param [in] keyID ID of the cryptographic key to associate to the new file, ignored if opening existing file.
 * @param [in] algo ID of the cryptographic algorithm to associate to the new file, ignored if opening existing file.
 * @return The function returns 0 in case of success. See the various error codes in SE3_FRESULT.
 */
SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, BYTE mode, uint32_t keyID, uint16_t algo);

/** @brief This function closes an open file.
 * @param [in] se_fp Pointer to an open file structure.
 * @return The function returns 0 in case of success. See the various error codes in SE3_FRESULT.
 */
SE3_FRESULT secure_close(SE3_FIL* se_fp);

/** @brief This function reads dataOut_len bytes into dataOut from an open file.
 * @param [in] se_fp Pointer to the open file to read from.
 * @param [out] dataOut An already allocated buffer used to store the read data.
 * @param [in] dataOut_len The amount of bytes we want to read.
 * @param [out] bytesRead Number of effective bytes read.
 * @return The function returns 0 in case of success. See the various error codes in SE3_FRESULT.
 */
SE3_FRESULT secure_read(SE3_FIL* se_fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead);

/** @brief This function writes the bytes stored at dataIn to an open file.
 * @param [in] se_fp Pointer to the open file to write to.
 * @param [in] dataIn The array of bytes that have to be written.
 * @param [in] dataIn_len The amount of bytes we want to write.
 * @return The function returns 0 in case of success. See the various error codes in SE3_FRESULT.
 */
SE3_FRESULT secure_write(SE3_FIL* se_fp, uint8_t *dataIn, uint32_t dataIn_len);

/** @brief This function is used to move the file pointer of an open file.
 * @param [in] se_fp Pointer to the open file whose file pointer we want to move.
 * @param [in] offset Amount of bytes we want to move.
 * @param [out] position Pointer to a uint32_t variable where the final position is stored, it cannot be NULL.
 * @param [in] whence According to this parameter we can choose if we want to move from the file beginning, file ending or current file pointer position. See \ref Seek_Defines.
 * @return The function returns 0 in case of success. See the various error codes in SE3_FRESULT.
 */
SE3_FRESULT secure_seek(SE3_FIL* se_fp, int64_t offset, uint32_t *position, uint8_t whence);


#endif /* SRC_DEVICE_SE3_FATFS_H_ */
