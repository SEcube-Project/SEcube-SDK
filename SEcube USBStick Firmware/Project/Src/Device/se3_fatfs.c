#include "se3_fatfs.h"

static bool find_and_read_key(uint32_t keyID, se3_fatfs_key* key);
static SE3_FRESULT crypto_filename(char *path, char *enc_name, uint16_t *encoded_length);
static void get_filename(char *path, char *file_name, int maxLength);
static void get_path(char *full_path, char *path);


SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo)
{
	SE3_FRESULT res;
	char enc_name[MAX_PATHNAME];
	uint16_t encoded_name_length;

	if(! find_and_read_key(keyID, &(se_fp->key)))
		return SE3_FR_NO_KEY;

	res = crypto_filename(path, enc_name, &encoded_name_length);
	if (res != SE3_FR_OK)
		return res;

	res = f_open(&(se_fp->fp), enc_name, mode);
	if (res != SE3_FR_OK)
		return res;



	return SE3_FR_OK;
}

SE3_FRESULT secure_read(SE3_FIL* fp, uint8_t *dataOut, uint32_t dataOut_len, uint32_t *bytesRead)
{
	return f_read(&(fp->fp), dataOut, dataOut_len, (uint*) bytesRead);
}

SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len)
{
	UINT bw;
	f_write(&(fp->fp), fp->key.data, SE3_FATFS_KEY_SIZE, &bw);
	return f_write(&(fp->fp), dataIn, dataIn_len, &bw);
}

SE3_FRESULT secure_close(SE3_FIL* fp)
{
	return f_close(&(fp->fp));
}

static
bool find_and_read_key(uint32_t keyID, se3_fatfs_key* key)
{
	se3_flash_it iterator;
	se3_flash_key flashKey;

	flashKey.data = key->data;
	memset(flashKey.data, 0, SE3_FATFS_KEY_SIZE);

	if (!se3_key_find(keyID, &iterator))
		return false;

	se3_key_read(&iterator, &flashKey);

	return true;
}

static
SE3_FRESULT crypto_filename(char *path, char *enc_name,
		uint16_t *encoded_length)
{
	if ((path == NULL) || (enc_name == NULL))
	{
		return SE3_FR_INVALID_NAME;
	}
	uint8_t orig_filename[_MAX_LFN+1];
	uint8_t bufferName[B5_SHA256_DIGEST_SIZE];
	uint8_t SHAName[2*B5_SHA256_DIGEST_SIZE+1];
	uint8_t dirPath[MAX_PATHNAME];
	uint16_t lorig, i;
	B5_tSha256Ctx ctx;
	int dirPathLen;

	get_filename(path, (char*) orig_filename, _MAX_LFN);
	get_path(path, (char*) dirPath);
	lorig = strnlen((const char*) orig_filename, _MAX_LFN);

	if ((B5_SHA256_RES_OK != B5_Sha256_Init(&ctx)))
	{
		return SE3_FR_FILENAME_ENC_ERROR;
	}
	if ((B5_SHA256_RES_OK != B5_Sha256_Update(&ctx, (uint8_t*) orig_filename,
			lorig * sizeof(uint8_t))))
	{
		return SE3_FR_FILENAME_ENC_ERROR;
	}
	if ((B5_SHA256_RES_OK != B5_Sha256_Finit(&ctx, (uint8_t*) bufferName)))
	{
		return SE3_FR_FILENAME_ENC_ERROR;
	}
	if (strlen((const char*) dirPath) > MAX_PATHNAME - 2*B5_SHA256_DIGEST_SIZE)
	{
		return SE3_FR_FILENAME_ENC_ERROR;
	}
	for (i = 0; i < B5_SHA256_DIGEST_SIZE; i++)
	{
		sprintf((char*) &(SHAName[i * 2]), "%02x", (uint8_t) bufferName[i]);
	}
	dirPathLen = strlen((const char*) dirPath);
	if (encoded_length != NULL)
		*encoded_length = i * 2 + dirPathLen;

	if (dirPath[0])
	{
		memcpy(enc_name, dirPath, dirPathLen);
	}
	memcpy(enc_name + dirPathLen, SHAName, 2*B5_SHA256_DIGEST_SIZE+1);
	return SE3_FR_OK;
}

static
void get_path(char *full_path, char *path)
{
	if ((full_path == NULL) || (path == NULL))
	{
		return;
	}
	char *p_name;
	p_name = strrchr(full_path, '/');
	if (p_name == NULL)
	{
		p_name = strrchr(full_path, '\\');
		if (p_name == NULL)
		{
			path[0] = '\0';
		}
		else
		{
			memcpy(path, full_path, p_name - full_path + 1);
			path[p_name - full_path + 1] = '\0';
		}
	}
	else
	{
		memcpy(path, full_path, p_name - full_path + 1);
		path[p_name - full_path + 1] = '\0';
	}
}

static
void get_filename(char *path, char *file_name, int maxLength)
{
	if ((file_name == NULL) || (path == NULL))
	{
		return;
	}
	char *f_name;
	f_name = strrchr(path, '/');
	if (f_name == NULL)
	{
		f_name = strrchr(path, '\\');
		if (f_name == NULL)
		{
			strncpy(file_name, path, maxLength);
		}
		else
		{
			strncpy(file_name, f_name + 1, maxLength);
		}
	}
	else
	{
		strncpy(file_name, f_name + 1, maxLength);
	}
}
