/*
 * hex2float.c - convert hex values to floats
 * Author: Will Eccles
 * Compile with -std=c89 -fno-strict-aliasing
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWAP(a,b) ((a)^=(b),(b)^=(a),(a)^=(b))

#define FALSE 0
#define TRUE 1

unsigned long swapbytes(unsigned long val)
{
	unsigned char *bytes;

	bytes = (unsigned char *)&val;

	SWAP(bytes[0], bytes[3]);
	SWAP(bytes[1], bytes[2]);

	return val;
}

void usage(const char *argv0)
{
	fprintf(stderr,
		"usage: %s [%%][0x]HEX\n"
		"  If the hex value begins with a %% character, it will be\n"
		"  interpreted as little-endian, and its byte order will be\n"
		"  swapped before converting.\n",
		argv0);
}

int main(int argc, char **argv)
{
	char *input, *end;
	int bswap;
	unsigned long ival;

	if (argc != 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	input = argv[1];
	end = NULL;
	bswap = FALSE;

	while (isspace(*input) && *input != '\0')
		input++;

	if (*input == '\0') {
		fprintf(stderr, "Empty input\n");
		exit(EXIT_FAILURE);
	}

	if (*input == '%') {
		bswap = TRUE;
		input++;
	}

	errno = 0;
	ival = strtol(input, &end, 16);

	if (errno != 0) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}

	if (end == input) {
		fprintf(stderr, "Invalid input: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if (bswap)
		ival = swapbytes(ival);

	printf("%f\n", *((float *)&ival));

	exit(EXIT_SUCCESS);
}
