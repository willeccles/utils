/*
 * resdiv.c - Compute resistive divider values
 * Author: Will Eccles
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	double r1, r2, vin, mult;

	if (argc < 3 || argc > 4) {
		fprintf(stderr, "usage: %s R1 R2 [Vin]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* FIXME: use strtod() */
	r1 = atof(argv[1]);
	r2 = atof(argv[2]);

	if (argc >= 4)
		vin = atof(argv[3]);

	mult = r2 / (r1 + r2);

	printf("voltage scale: %g\n", mult);

	if (argc >= 4)
		printf("output voltage (with Vin = %g): %g\n", vin, mult * vin);

	exit(EXIT_SUCCESS);
}

/* vim: set noet sw=8 : */
