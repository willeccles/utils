/*
 * jedcrc.c - Compute the checksum of the transmission data in a JEDEC .jed file
 * Author: Will Eccles
 * Compile with -std=c89
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

#define STX 0x02
#define ETX 0x03

int main(int argc, char **argv)
{
	FILE *src;
	unsigned short sum;
	int c;
	int start, end;

	if (argc != 2) {
		fprintf(stderr, "usage: %s PATH\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[1], "-") == 0) {
		src = stdin;
	} else {
		src = fopen(argv[1], "rb");
		if (src == NULL) {
			perror(argv[1]);
			exit(EXIT_FAILURE);
		}
	}

	sum = 0;
	start = FALSE;
	end = FALSE;
	while (!((c = fgetc(src)) & ~0x7F)) {
		if (!start && c == STX)
			start = TRUE;

		if (start && !end)
			sum += (unsigned short)c;

		if (!end && c == ETX) {
			end = TRUE;
			break;
		}
	}

	if (src != stdin)
		fclose(src);

	if (start && end) {
		printf("Checksum: %04hX\n", sum);
	} else if (!start) {
		fprintf(stderr, "No STX byte found\n");
		exit(EXIT_FAILURE);
	} else if (!end) {
		fprintf(stderr, "No ETX byte found\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
