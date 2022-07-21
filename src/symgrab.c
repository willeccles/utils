/*
 * symgrab.c - load a symbol of a given type from an object file
 * Author: Will Eccles
 * Compile with -std=c99
 * Link with -ldl
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum symbol_type {
	SYM_INT8,
	SYM_INT16,
	SYM_INT32,
	SYM_INT64,
	SYM_UINT8,
	SYM_UINT16,
	SYM_UINT32,
	SYM_UINT64,
	/* X = hex */
	SYM_XINT8,
	SYM_XINT16,
	SYM_XINT32,
	SYM_XINT64,
	SYM_FLOAT,
	SYM_DOUBLE,
	SYM_STRING,

	SYM_INVALID
};

void usage(char *argv0)
{
	fprintf(stderr,
		"usage: %s  FILE  SYMBOL  TYPE\n\n"
		"  FILE\t\tan object file (.o, .so)\n"
		"  SYMBOL\ta symbol name\n"
		"  type\t\tthe type of the symbol:\n"
		"\t\t - i8, i16, i32, i64\n"
		"\t\t - u8, u16, u32, u64\n"
		"\t\t - x8, x16, x32, x64\n"
		"\t\t - [f]loat, [d]ouble\n"
		"\t\t - [s]tring\n",
		argv0);
}

enum symbol_type parse_type(const char *typearg)
{
	if (strlen(typearg) == 1) {
		switch (typearg[0]) {
		case 'f':
			return SYM_FLOAT;
		case 'd':
			return SYM_DOUBLE;
		case 's':
			return SYM_STRING;
		default:
			return SYM_INVALID;
		}
	}

	if (strcmp(typearg, "string") == 0)
		return SYM_STRING;
	else if (strcmp(typearg, "float") == 0)
		return SYM_FLOAT;
	else if (strcmp(typearg, "double") == 0)
		return SYM_DOUBLE;
	else if (strcmp(typearg, "i8") == 0)
		return SYM_INT8;
	else if (strcmp(typearg, "i16") == 0)
		return SYM_INT16;
	else if (strcmp(typearg, "i32") == 0)
		return SYM_INT32;
	else if (strcmp(typearg, "i64") == 0)
		return SYM_INT64;
	else if (strcmp(typearg, "u8") == 0)
		return SYM_UINT8;
	else if (strcmp(typearg, "u16") == 0)
		return SYM_UINT16;
	else if (strcmp(typearg, "u32") == 0)
		return SYM_UINT32;
	else if (strcmp(typearg, "u64") == 0)
		return SYM_UINT64;
	else if (strcmp(typearg, "x8") == 0)
		return SYM_XINT8;
	else if (strcmp(typearg, "x16") == 0)
		return SYM_XINT16;
	else if (strcmp(typearg, "x32") == 0)
		return SYM_XINT32;
	else if (strcmp(typearg, "x64") == 0)
		return SYM_XINT64;
	else
		return SYM_INVALID;
}

void print_value(enum symbol_type valtype, void *valptr)
{
#define PTYPE(symtype, type, fmt)				\
	do {							\
		if (valtype == symtype) {			\
			printf("%" fmt "\n", *(type *)valptr);	\
			return;					\
		}						\
	} while (0)

	PTYPE(SYM_INT8,   char,               "hhd");
	PTYPE(SYM_INT16,  short,              "hd");
	PTYPE(SYM_INT32,  int,                "d");
	PTYPE(SYM_INT64,  long long,          "lld");
	PTYPE(SYM_UINT8,  unsigned char,      "hhu");
	PTYPE(SYM_UINT16, unsigned short,     "hu");
	PTYPE(SYM_UINT32, unsigned int,       "u");
	PTYPE(SYM_UINT64, unsigned long long, "llu");
	PTYPE(SYM_XINT8,  unsigned char,      "hhx");
	PTYPE(SYM_XINT16, unsigned short,     "hx");
	PTYPE(SYM_XINT32, unsigned int,       "x");
	PTYPE(SYM_XINT64, unsigned long long, "llx");
	PTYPE(SYM_FLOAT,  float,              "g");
	PTYPE(SYM_DOUBLE, double,             "g");
	PTYPE(SYM_STRING, char *,             "s");
}

int main(int argc, char **argv)
{
	const char *filename, *symbol;
	enum symbol_type type;
	void *handle, *sym;

	if (argc != 4) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	filename = argv[1];
	symbol = argv[2];

	if (access(filename, F_OK) != 0) {
		perror(filename);
		exit(EXIT_FAILURE);
	}

	type = parse_type(argv[3]);
	if (type == SYM_INVALID) {
		fprintf(stderr, "invalid type: %s\n", argv[3]);
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	handle = dlopen(filename, RTLD_NOW);
	if (handle == NULL) {
		fprintf(stderr, "error opening file: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	/* clear errors, if any */
	dlerror();

	sym = dlsym(handle, symbol);
	if (sym == NULL) {
		fprintf(stderr, "error loading symbol: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	printf("symbol %s found at %p:\n", argv[2], sym);
	print_value(type, sym);

	dlclose(handle);

	exit(EXIT_SUCCESS);
}
