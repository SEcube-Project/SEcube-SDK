#include "se3_fatfs.h"

static bool find_and_read_key(uint32_t keyID, se3_flash_key* key);

SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo)
{
	if(! find_and_read_key(keyID, &(se_fp->key)))
		return FR_NO_KEY;

	return FR_OK;
}

static
bool find_and_read_key(uint32_t keyID, se3_flash_key* key)
{
	se3_flash_it iterator;
	if (!se3_key_find(keyID, &iterator))
		return false;

	se3_key_read(&iterator, key);

	return true;
}
