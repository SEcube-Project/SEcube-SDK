#include "se3_fatfs.h"

static bool find_and_read_key(uint32_t keyID, se3_flash_key* key);
static SE3_FRESULT crypto_filename(char *path, char *enc_name, uint16_t *encoded_length);
static void get_filename(char *path, char *file_name);
static void get_path(char *full_path, char *path);


SE3_FRESULT secure_open(SE3_FIL* se_fp, char *path, int32_t mode, int32_t creation, uint32_t keyID, uint16_t algo)
{
	SE3_FRESULT res;
	char enc_name[MAX_PATHNAME];
	uint16_t encoded_name_length;


	//if(! find_and_read_key(keyID, &(se_fp->key)))
	//	return SE3_FR_NO_KEY;

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
	return f_read(&(fp->fp), dataOut, dataOut_len, bytesRead);
}

SE3_FRESULT secure_write(SE3_FIL* fp, uint8_t *dataIn, uint32_t dataIn_len)
{
	UINT bw;
	return f_write(&(fp->fp), dataIn, dataIn_len, &bw);
}

SE3_FRESULT secure_close(SE3_FIL* fp)
{
	return f_close(&(fp->fp));
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

static
SE3_FRESULT crypto_filename(char *path, char *enc_name,
		uint16_t *encoded_length)
{
	if ((path == NULL) || (enc_name == NULL))
	{
		return SE3_FR_INVALID_NAME;
	}
	uint16_t commandError = 0;
	uint8_t orig_filename[_MAX_LFN];
	uint8_t bufferName[MAX_PATHNAME];
	uint8_t SHAName[MAX_PATHNAME];
	uint8_t finalPath[MAX_PATHNAME];
	uint16_t lorig = 0, i = 0;
	B5_tSha256Ctx ctx;
	int finalLen = 0;
	memset(orig_filename, 0, MAX_PATHNAME * sizeof(char));
	memset(bufferName, 0, MAX_PATHNAME * sizeof(char));
	memset(SHAName, 0, MAX_PATHNAME * sizeof(char));
	memset(finalPath, 0, MAX_PATHNAME * sizeof(char));
	get_filename(path, (char*) orig_filename);
	get_path(path, (char*) finalPath);
	lorig = strlen((const char*) orig_filename);
	if ((commandError = B5_Sha256_Init(&ctx)))
	{
		return commandError;
	}
	if ((commandError = B5_Sha256_Update(&ctx, (uint8_t*) orig_filename,
			lorig * sizeof(uint8_t))))
	{
		return commandError;
	}
	if ((commandError = B5_Sha256_Finit(&ctx, (uint8_t*) bufferName)))
	{
		return commandError;
	}
	if (strlen((const char*) finalPath) > MAX_PATHNAME - B5_SHA256_DIGEST_SIZE)
	{
		return SE3_FR_FILENAME_ENC_ERROR;
	}
	for (i = 0; i < B5_SHA256_DIGEST_SIZE; i++)
	{
		sprintf((char*) &(SHAName[i * 2]), "%02x", (uint8_t) bufferName[i]);
	}
	finalLen = strlen((const char*) finalPath);
	if (encoded_length != NULL)
		*encoded_length = i * 2 + finalLen;
	if (finalPath[0])
	{
		memcpy(enc_name, finalPath, finalLen);
	}
	memcpy(enc_name + finalLen, SHAName, i * 2 + 1);
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
void get_filename(char *path, char *file_name)
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
			strncpy(file_name, path, MAX_PATHNAME);
		}
		else
		{
			strncpy(file_name, f_name + 1, MAX_PATHNAME);
		}
	}
	else
	{
		strncpy(file_name, f_name + 1, MAX_PATHNAME);
	}
}
