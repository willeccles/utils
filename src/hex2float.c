#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>

#define SWAP(a,b) ((a)^=(b),(b)^=(a),(a)^=(b))

uint32_t swapbytes(uint32_t val) {
  uint8_t* bytes = (uint8_t*)&val;
  SWAP(bytes[0], bytes[3]);
  SWAP(bytes[1], bytes[2]);
  return val;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr,
            "usage: %s [%%][0x]HEX\n"
            "  If the hex value begins with a %% character, it will be\n"
            "  interpreted as little-endian, and its byte order will be\n"
            "  swapped before converting.\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  char* input = argv[1];
  char* end = NULL;
  bool bswap = false;
  uint32_t ival;

  while (isspace(*input) && *input != '\0') {
    input++;
  }

  if (*input == '\0') {
    fprintf(stderr, "Empty input\n");
    exit(EXIT_FAILURE);
  }

  if (*input == '%') {
    bswap = true;
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

  if (bswap) {
    ival = swapbytes(ival);
  }

  printf("%f\n", *((float*)&ival));

  exit(EXIT_SUCCESS);
}
