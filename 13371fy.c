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

#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

enum C_CASE { C_CASE_NONE, C_CASE_LOWER, C_CASE_UPPER };
static int HELP_FLAG = 0;

static struct option long_options[] = 
{
	{"help", no_argument, &HELP_FLAG, 1},
	{"in", required_argument, 0, 'i'},
	{"out", required_argument, 0, 'o'},
	{0, 0, 0, 0}
};

bool
file_exists(const char *fname)
{
	FILE *file;
	if ((file = fopen(fname, "r"))) {
		fclose(file);
		return true;
	}
	return false;
}

bool
is_1337(FILE *file_in)
{
	char c;
	if ('\0' == (c = fgetc(file_in)))
		return true;

	ungetc(c, file_in);
	return false;
}

void
normiefy(FILE *file_in, FILE *file_out)
{
	char c;
	int c_case;

	while (EOF != (c = fgetc(file_in))) {
		switch (c) {
		case '\x0e':
			c_case = C_CASE_LOWER;
			c = fgetc(file_in);
			break;
		case '\x0f':
			c_case = C_CASE_UPPER;
			c = fgetc(file_in);
			break;
		default:
			c_case = C_CASE_NONE;
		}

		if (C_CASE_NONE != c_case && EOF != c) {
			switch (c) {
			case '4':
				c = 'A';
				break;
			case '3':
				c = 'E';
				break;
			case '1':
				c = 'I';
				break;
			case '0':
				c = 'O';
				break;
			case '5':
				c = 'S';
				break;
			case '7':
				c = 'T';
				break;
			}

			if (C_CASE_LOWER == c_case)
				c |= 0x20;
		}
		fputc(c, file_out);
	}
}

void
l3371fy(FILE *file_in, FILE *file_out)
{
	char c, c_prime;
	fputc('\0', file_out);

	while (EOF != (c = fgetc(file_in))) {
		c_prime = c;
		switch (c_prime) {
		case 'A':
		case 'a':
			c_prime = '4';
			break;
		case 'E':
		case 'e':
			c_prime = '3';
			break;
		case 'I':
		case 'i':
			c_prime = '1';
			break;
		case 'O':
		case 'o':
			c_prime = '0';
			break;
		case 'S':
		case 's':
			c_prime = '5';
			break;
		case 'T':
		case 't':
			c_prime = '7';
			break;
		}
		
		if (c != c_prime) {
			if (isupper(c))
				fputc('\x0f', file_out);
			else
				fputc('\x0e', file_out);
		}

		fputc(c_prime, file_out);
	}
}

void
print_usage(const char *program_name)
{
	fprintf(stderr, "Usage: %s [-i input_file] [-o output_file]\n", program_name);
}

int
main(int argc, char **argv)
{
	char user_response;
	int arg;
	int option_index = 0;
	int return_value = -1;
	FILE *file_in = stdin;
	FILE *file_out = stdout;
	char *file_in_string = NULL;
	char *file_out_string = NULL;

	while (-1 != (arg = getopt_long(argc, argv, "i:o:h", long_options, &option_index))) {
		switch (arg) {
		case 0:
			if (HELP_FLAG) {
				print_usage(argv[0]);
				return_value = 0;
				goto done;
			}
			fprintf(stderr, "Unknown option '%s'.\n", long_options[option_index].name);
			goto done;
		case 'h':
			print_usage(argv[0]);
			goto done;
		case 'i':
			file_in_string = optarg;
			break;
		case 'o':
			file_out_string = optarg;
			break;
		case '?':
			/* case 0 handles superfluous arguments. */
			break;
		}

	}

	/* Check if output file exists and prompt user accordingly. */
	if (NULL != file_out_string) {
		if (true == file_exists(file_out_string)) {
			fprintf(stderr, "Output file \"%s\" already exists. Overwrite? [N/y]: ", file_out_string);
			user_response = getchar();
			if ('Y' != (user_response & ~0x20)) {
				fprintf(stderr, "Aborting.\n");
				goto done;
			}
		}
	}

	/* Open input file for reading. */
	if (NULL != file_in_string) {
		if (NULL == (file_in = fopen(file_in_string, "r"))) {
			fprintf(stderr, "Unable to open input file \"%s\" for reading.\n", file_in_string);
			goto done;
		}
	}

	/* Open output file for writing. */
	if (NULL != file_out_string) {
		if (NULL == (file_out = fopen(file_out_string, "w"))) {
			fprintf(stderr, "Unable to open output file \"%s\" for writing.\n", file_out_string);
			goto close_in;
		}
	}

	if (is_1337(file_in))
		normiefy(file_in, file_out);
	else /* t0t41 pwn4g3!!11!1 */
		l3371fy(file_in, file_out);

	return_value = 0;

	fclose(file_out);
close_in:
	fclose(file_in);
done:
	return return_value;
}
