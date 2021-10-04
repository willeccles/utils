#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

enum SymbolType {
  kSymInt8,
  kSymInt16,
  kSymInt32,
  kSymInt64,
  kSymUInt8,
  kSymUInt16,
  kSymUInt32,
  kSymUInt64,
  kSymXInt8,
  kSymXInt16,
  kSymXInt32,
  kSymXInt64,

  kSymFloat,
  kSymDouble,
  kSymString,

  kSymInvalid,
};

void usage(char* argv0) {
  fprintf(stderr,
          "usage: %s objfile symbol type\n\n"
          "  objfile: an object file (.o, .so)\n"
          "  symbol:  the name of the symbol to read\n"
          "  type:    the type of the symbol, which is one of:\n"
          "             - i8, i16, i32, i64 (signed integers)\n"
          "             - u8, u16, u32, u64 (unsigned integers)\n"
          "             - x8, x16, x32, x64 (unsigned integers as hex)\n"
          "             - [f]loat, [d]ouble (float and double)\n"
          "             - [s]tring          (null-terminated string)\n",
          argv0);
}

enum SymbolType ParseType(const char* typearg) {
  if (strlen(typearg) == 1) {
    switch (typearg[0]) {
      case 'f':
        return kSymFloat;
      case 'd':
        return kSymDouble;
      case 's':
        return kSymString;
      default:
        return kSymInvalid;
    }
  } else if (strcmp(typearg, "string") == 0) {
    return kSymString;
  } else if (strcmp(typearg, "float") == 0) {
    return kSymFloat;
  } else if (strcmp(typearg, "double") == 0) {
    return kSymDouble;
  } else if (strcmp(typearg, "i8") == 0) {
    return kSymInt8;
  } else if (strcmp(typearg, "i16") == 0) {
    return kSymInt16;
  } else if (strcmp(typearg, "i32") == 0) {
    return kSymInt32;
  } else if (strcmp(typearg, "i64") == 0) {
    return kSymInt64;
  } else if (strcmp(typearg, "u8") == 0) {
    return kSymUInt8;
  } else if (strcmp(typearg, "u16") == 0) {
    return kSymUInt16;
  } else if (strcmp(typearg, "u32") == 0) {
    return kSymUInt32;
  } else if (strcmp(typearg, "u64") == 0) {
    return kSymUInt64;
  } else if (strcmp(typearg, "x8") == 0) {
    return kSymXInt8;
  } else if (strcmp(typearg, "x16") == 0) {
    return kSymXInt16;
  } else if (strcmp(typearg, "x32") == 0) {
    return kSymXInt32;
  } else if (strcmp(typearg, "x64") == 0) {
    return kSymXInt64;
  } else {
    return kSymInvalid;
  }
}

void PrintValue(enum SymbolType valtype, void* valptr) {
  switch (valtype) {
#define PTYPE(symtype, type, fmt) case symtype:\
    printf("%" fmt "\n", *(type*)valptr);\
    break;
    PTYPE(kSymInt8, int8_t, PRId8);
    PTYPE(kSymInt16, int16_t, PRId16);
    PTYPE(kSymInt32, int32_t, PRId32);
    PTYPE(kSymInt64, int64_t, PRId64);
    PTYPE(kSymUInt8, uint8_t, PRIu8);
    PTYPE(kSymUInt16, uint16_t, PRIu16);
    PTYPE(kSymUInt32, uint32_t, PRIu32);
    PTYPE(kSymUInt64, uint64_t, PRIu64);
    PTYPE(kSymXInt8, uint8_t, PRIx8);
    PTYPE(kSymXInt16, uint16_t, PRIx16);
    PTYPE(kSymXInt32, uint32_t, PRIx32);
    PTYPE(kSymXInt64, uint64_t, PRIx64);
    PTYPE(kSymFloat, float, "g");
    PTYPE(kSymDouble, double, "g");
    PTYPE(kSymString, char*, "s");
    default:
      break;
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  const char* filename = argv[1];
  const char* symbol = argv[2];

  if (access(filename, F_OK) != 0) {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  enum SymbolType type = ParseType(argv[3]);

  if (type == kSymInvalid) {
    fprintf(stderr, "Invalid type: %s\n", argv[3]);
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  void* handle = dlopen(filename, RTLD_NOW);
  if (!handle) {
    fprintf(stderr, "Error opening file: %s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  // clear errors if any
  dlerror();

  void* sym = dlsym(handle, symbol);
  if (!sym) {
    fprintf(stderr, "Error loading symbol: %s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  printf("Symbol %s found at %p:\n", argv[2], sym);
  PrintValue(type, sym);

  dlclose(handle);

  exit(EXIT_SUCCESS);
}
