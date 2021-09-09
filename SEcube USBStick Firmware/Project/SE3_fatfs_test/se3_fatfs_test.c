#include "se3_fatfs_test.h"

#define PAYLOAD_SIZE 2368

int test_write_read();
int execute_test(int (test_function)(), char* test_name);
int test_write_readless();
int test_open_nokey_noalgo();
int test_write_readmore();
int test_write_read_sector_multiple();
int test_seek();
int test_invalid_file();
int test_unopen_file();
int test_negative_seek();
int test_write_read_dir();
int test_open_not_existing();
int test_seek_out_of_dirty_sector();

uint8_t payload[PAYLOAD_SIZE] =
				"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus eu metus augue. Maecenas justo felis, mollis et erat a, viverra ullamcorper quam. Ut urna felis, tempus imperdiet ornare auctor, iaculis non sapien. Integer luctus, erat in hendrerit imperdiet, augue tellus placerat orci, ut egestas sapien ipsum non massa. Pellentesque ut laoreet erat, sit amet sagittis nisi. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Morbi sit amet nisl tincidunt ex porta sollicitudin eu vitae turpis. Vestibulum ut blandit dui. Nunc eu diam maximus, imperdiet nulla sodales, ultrices enim.Mauris ornare eros elementum arcu viverra hendrerit. Vestibulum urna ligula, auctor a dapibus id, tincidunt ut nunc. Aliquam non dolor ligula. Mauris maximus lorem diam, vitae dignissim ante lobortis a. Quisque sollicitudin lorem vitae sapien tincidunt, quis luctus lorem tincidunt. Integer dolor enim, viverra vitae pharetra sit amet, porttitor eu urna. Nullam vel laoreet ipsum, nec fringilla orci. Phasellus convallis malesuada nulla, quis sagittis sapien tincidunt a. Nunc at sagittis dui, sed blandit ante.Donec interdum nisi et pellentesque lacinia. Vestibulum ultricies ultrices erat in luctus. Maecenas nec turpis sit amet ex posuere feugiat. Donec sed bibendum nulla. Ut eu nunc bibendum, interdum massa tempus, ornare lorem. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Integer augue magna, pulvinar vel metus sed, blandit finibus elit.Integer tincidunt neque in blandit cursus. Donec eu vestibulum dolor. Etiam consectetur eros nulla, faucibus malesuada libero aliquam ut. Nam est ligula, porta sit amet porttitor vitae, varius non neque. Duis gravida sed orci at pellentesque. Donec lectus ligula, pellentesque nec erat rhoncus, cursus tincidunt massa. Etiam commodo luctus quam, auctor vulputate massa sodales in. Vestibulum ac suscipit sapien, a viverra mi.Nam eu metus magna. Duis mollis turpis ante. Interdum et malesuada fames ac ante ipsum primis in faucibus. Praesent eget sem a odio dapibus mattis vitae a nisl. Donec a lorem ante. Aliquam quis accumsan neque, id vulputate est. Morbi sit amet ante aliquet, viverra lacus et, posuere nisl. Suspendisse eu mauris tellus.Suspendisse sed quam nec nulla blandit molestie sit amet in ligula. Pellentesque venenatis dapibus lectus. ";
FIL test_report;


int begin_test()
{
	se3_flash_it f_it;
	se3_flash_key key;
	FATFS fs;

	uint8_t keyData[] = "pro123vapro123va";

	uint16_t tests_failed;


	f_mount (&fs, "", 0);

	tests_failed = 0;
	key.id = 1;
	key.data = keyData;
	key.data_size = 16;

	if (!se3_key_find(1, &f_it))
		se3_key_new(&f_it, &key);

	f_open(&test_report, "test_report.txt", FA_OPEN_ALWAYS | FA_WRITE);

	f_printf(&test_report, "Built in date: %s at %s\n", __DATE__, __TIME__);

	tests_failed += execute_test(test_write_read, "test_write_read");
	tests_failed += execute_test(test_write_readless, "test_write_read_less");
	tests_failed += execute_test(test_open_nokey_noalgo, "test_open_nokey_noalgo");
	tests_failed += execute_test(test_write_readmore, "test_write_readmore");
	tests_failed += execute_test(test_write_read_sector_multiple, "test_write_read_sector_multiple");
	tests_failed += execute_test(test_seek, "test_seek");
	tests_failed += execute_test(test_invalid_file, "test_invalid_file");
	tests_failed += execute_test(test_unopen_file, "test_unopen_file");
	tests_failed += execute_test(test_negative_seek, "test_negative_seek");
	tests_failed += execute_test(test_write_read_dir, "test_write_read_dir");
	tests_failed += execute_test(test_open_not_existing, "test_open_not_existing");
	tests_failed += execute_test(test_seek_out_of_dirty_sector, "test_seek_out_of_dirty_sector");

	f_close(&test_report);
	return tests_failed;
}

int execute_test(int (test_function)(), char* test_name)
{
	int res;

	f_printf(&test_report, test_name);
	if ( (res = (test_function())) )
		f_printf(&test_report, "(): FAIL\n");
	else
		f_printf(&test_report, "(): PASS\n");

	return res;
}

int test_write_read()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, PAYLOAD_SIZE);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_read(&fp, buffOut, PAYLOAD_SIZE, &bytes_read);
	res += secure_close(&fp);

	if (res)
		return TEST_FAIL;

	if (memcmp(payload, buffOut, PAYLOAD_SIZE))
		return TEST_FAIL;

	return TEST_PASS;
}

int test_write_readless()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, PAYLOAD_SIZE);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_read(&fp, buffOut, 1200, &bytes_read);
	if (bytes_read != 1200)
		return TEST_FAIL;
	res += secure_close(&fp);

	if (res)
		return TEST_FAIL;

	if (memcmp(payload, buffOut, 1200))
		return TEST_FAIL;

	return TEST_PASS;
}

int test_write_readmore()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, 1200);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_read(&fp, buffOut, 3000, &bytes_read);
	if (bytes_read != 1200)
		return TEST_FAIL;
	res += secure_close(&fp);

	if (res)
		return TEST_FAIL;

	if (memcmp(payload, buffOut, 1200))
		return TEST_FAIL;

	return TEST_PASS;
}

int test_open_nokey_noalgo()
{
	SE3_FRESULT res;
	SE3_FIL fp1,fp2;
	res = secure_open(&fp1, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 2, SE3_ALGO_AES_HMACSHA256);
	if (res != SE3_FR_NO_KEY)
		return TEST_FAIL;

	res = secure_open(&fp2, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, 8);
	if (res != SE3_FR_INVALID_ALGO)
		return TEST_FAIL;

	return TEST_PASS;
}

int test_write_read_sector_multiple()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, 2*SE3_FATFS_LOGIC_DATA);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_read(&fp, buffOut, PAYLOAD_SIZE, &bytes_read);
	if (bytes_read != 2*SE3_FATFS_LOGIC_DATA)
		return TEST_FAIL;
	res += secure_close(&fp);

	if (res)
		return TEST_FAIL;

	if (memcmp(payload, buffOut, 2*SE3_FATFS_LOGIC_DATA))
		return TEST_FAIL;

	return TEST_PASS;
}

int test_seek()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;
	uint32_t position;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, PAYLOAD_SIZE);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_seek(&fp, 100, &position, SE3_FATFS_END);
	if (position != PAYLOAD_SIZE)
		return TEST_FAIL;
	res += secure_seek(&fp, -500, &position, SE3_FATFS_END);
	res += secure_read(&fp, buffOut, 2000, &bytes_read);
	if(bytes_read != 500)
		return TEST_FAIL;
	res += secure_close(&fp);

	if(memcmp(buffOut, payload + PAYLOAD_SIZE-500, 500))
		return TEST_FAIL;

	if (res)
		return TEST_FAIL;

	return TEST_PASS;
}

int test_invalid_file()
{
	uint8_t buffOut[5000];
	FIL fp;
	SE3_FIL se_fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	res = secure_open(&se_fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res = secure_write(&se_fp, (void *) payload, PAYLOAD_SIZE);
	res = secure_close(&se_fp);

	res = f_open(&fp, "5f3fd179339b905bff378e5c33bcb265db3d427a41c337c4885b32041c0d2465", FA_WRITE);
	f_lseek(&fp, f_size(&fp)-1400);
	res = f_write(&fp, (void *) payload, 1014, NULL);
	res = f_close(&fp);

	res = secure_open(&se_fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res = secure_read(&se_fp, buffOut, 5000, &bytes_read);
	if (res != SE3_FR_INVALID_SIGNATURE)
		return TEST_FAIL;
	secure_close(&se_fp);

	res = f_open(&fp, "5f3fd179339b905bff378e5c33bcb265db3d427a41c337c4885b32041c0d2465", FA_WRITE);
	res = f_lseek(&fp, f_size(&fp));
	res = f_write(&fp, (void *) payload, 10, NULL);
	res = f_close(&fp);

	res = secure_open(&se_fp, "prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	if (res != SE3_FR_DATA_ENC_ERROR)
		return TEST_FAIL;

	return TEST_PASS;
}

int test_unopen_file()
{
	uint8_t buffOut[5000];
	SE3_FIL se_fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	if ((res = secure_write(&se_fp, (void *) payload, PAYLOAD_SIZE)) != SE3_FR_INVALID_OBJECT)
		return TEST_FAIL;
	res = secure_read(&se_fp, buffOut, 5000, &bytes_read);
	res = secure_close(&se_fp);

	return TEST_PASS;
}

int test_negative_seek()
{
	SE3_FIL se_fp;
	uint32_t position;

	secure_open(&se_fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);

	secure_write(&se_fp, (void *) payload, PAYLOAD_SIZE);
	if(secure_seek(&se_fp, -40000, &position, SE3_FATFS_END) != SE3_FR_SEEK_ERROR)
		return TEST_FAIL;
	secure_close(&se_fp);

	return TEST_PASS;
}

int test_write_read_dir()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;

	res = 0;

	f_mkdir("dir");

	res += secure_open(&fp, "dir/prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, PAYLOAD_SIZE);
	res += secure_close(&fp);

	res += secure_open(&fp, "dir/prova3.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_read(&fp, buffOut, PAYLOAD_SIZE, &bytes_read);
	res += secure_close(&fp);

	if (res)
		return TEST_FAIL;

	if (memcmp(payload, buffOut, PAYLOAD_SIZE))
		return TEST_FAIL;

	return TEST_PASS;
}

int test_open_not_existing()
{
	SE3_FIL fp;
	if (secure_open(&fp, "not_existing.txt", FA_READ, 1, SE3_ALGO_AES_HMACSHA256) != SE3_FR_NO_FILE)
		return TEST_FAIL;

	return TEST_PASS;
}

int test_seek_out_of_dirty_sector()
{
	uint8_t buffOut[3000];
	memset(buffOut, 1, 3000);
	SE3_FIL fp;
	SE3_FRESULT res;
	uint32_t bytes_read;
	uint32_t position;

	res = 0;

	res += secure_open(&fp, "prova3.txt", FA_CREATE_ALWAYS | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_write(&fp, (void *) payload, PAYLOAD_SIZE);
	res += secure_close(&fp);

	res += secure_open(&fp, "prova3.txt", FA_READ | FA_WRITE, 1, SE3_ALGO_AES_HMACSHA256);
	res += secure_seek(&fp, -10, &position, SE3_FATFS_END);
	res += secure_write(&fp, (void*)"0123456789", 10);
	res += secure_seek(&fp, -500, &position, SE3_FATFS_END);
	res += secure_read(&fp, buffOut, 2000, &bytes_read);
	if(bytes_read != 500)
		return TEST_FAIL;
	res += secure_close(&fp);

	if(memcmp(buffOut, payload + PAYLOAD_SIZE-500, 500-10))
		return TEST_FAIL;

	if(memcmp(buffOut+500-10, "0123456789", 10))
		return TEST_FAIL;

	if (res)
		return TEST_FAIL;

	return TEST_PASS;
}
