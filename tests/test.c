/*
 * Copyright (C) 2017 I-am-Batman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#define SHA256_TEXT_LEN 65

int TOTAL_TESTS = 0;
int PASSED_TESTS = 0;
unsigned char GOOD_HASH[SHA256_DIGEST_LENGTH];
unsigned char TEST_HASH[SHA256_DIGEST_LENGTH];

void
print_sha256(const char *path, unsigned char *hash_buf)
{
	printf("SHA256 of %s: ", path);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		printf("%02x", hash_buf[i]);
	}
	printf("\n");
}

bool
sha256_file(const char *path, unsigned char *hash_buf)
{
	int err;
	FILE *f;
	struct stat st;
	int bytes_read;
	off_t size;
	char *buf;
	SHA256_CTX sha256;
	
	err = stat(path, &st);

	if (-1 == err) {
		fprintf(stderr, "Unable to stat %s.\n", path);
		exit(-1);
	}

	size = st.st_size;

	f = fopen(path, "r");

	if (NULL == f) {
		fprintf(stderr, "Unable to open %s.\n", path);
		exit(-1);
	}

	buf = malloc(size);
	if (NULL == buf) {
		fprintf(stderr, "Unable to malloc file buffer.\n");
		exit(-1);
	}

	SHA256_Init(&sha256);
	while ((bytes_read = fread(buf, 1, size, f))) {
		SHA256_Update(&sha256, buf, bytes_read);
	}
	fclose(f);
	SHA256_Final(hash_buf, &sha256);

#ifdef DEBUG
	print_sha256(path, hash_buf);
#endif

	free(buf);
	return 0;	
}

bool
init_good_hash(char *file)
{
	return sha256_file(file, GOOD_HASH);
}

int
run_test(const char *cmd, const char *outfile, const unsigned char *good_hash, int good_exit)
{

	int cmd_exit;
	int ret = -1;

	TOTAL_TESTS++;
	cmd_exit = system(cmd);

	if (good_exit != cmd_exit) {
		printf("Failed test: `%s` returned %i but should have returned %i.\n", cmd, cmd_exit, good_exit);
		goto done;
	}

	if (NULL != good_hash) {
		sha256_file(outfile, TEST_HASH);
		if (memcmp(good_hash, TEST_HASH, SHA256_DIGEST_LENGTH)) {
			printf("Failed test: `%s` output file hash did not match good hash.\n", cmd);
			goto done;
		}
	}

	PASSED_TESTS++;
	printf("Passed test %i: `%s`\n", TOTAL_TESTS, cmd);
	ret = 0;

done:
	unlink(outfile);
	return ret;
}

int
main(int argc, char **argv)
{
	int err;
	int ret = -1;

	err = chdir("tests");
	if (0 != err) {
		fprintf(stderr, "%s: unable to chdir to `tests` directory\n", argv[0]);
		goto done;
	}

	init_good_hash("1337f1n.txt");

	/* Test input from stdin and output to stdout */
	run_test("cat huckfinn.txt | ../13371fy > test_out.tmp", "test_out.tmp", GOOD_HASH, 0);

	/* Test input from stdin and ouput to file */
	run_test("cat huckfinn.txt | ../13371fy -o test_out.tmp", "test_out.tmp", GOOD_HASH, 0);
	run_test("cat huckfinn.txt | ../13371fy --out test_out.tmp", "test_out.tmp", GOOD_HASH, 0);

	/* Test input from file and ouput to stdout */
	run_test("../13371fy -i huckfinn.txt > test_out.tmp", "test_out.tmp", GOOD_HASH, 0);
	run_test("../13371fy --in huckfinn.txt > test_out.tmp", "test_out.tmp", GOOD_HASH, 0);

	/* Test input from file and output to file */
	run_test("../13371fy -i huckfinn.txt -o test_out.tmp", "test_out.tmp", GOOD_HASH, 0);
	run_test("../13371fy --in huckfinn.txt -o test_out.tmp", "test_out.tmp", GOOD_HASH, 0);
	run_test("../13371fy -i huckfinn.txt --out test_out.tmp", "test_out.tmp", GOOD_HASH, 0);
	run_test("../13371fy --in huckfinn.txt --out test_out.tmp", "test_out.tmp", GOOD_HASH, 0);

	printf("PASSED %i/%i tests\n", PASSED_TESTS, TOTAL_TESTS);

	ret = 0;

done:
	return ret;
}
