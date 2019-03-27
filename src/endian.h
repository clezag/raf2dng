#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stdlib.h>
#include "type.h"

// from big endian. Reads a big endian value and converts it to local endianness
void fbe(void* p_buf, size_t size, size_t blocks);
#define FBE(x) fbe(&x, sizeof(x), 1)

#endif