#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stdlib.h>
#include "util.h"

// from/to big endian. If local endianness is not big, flips the endianness 
void fbe(void* p_buf, size_t size, size_t blocks);
#define FBE(x) fbe(&x, sizeof(x), 1)

#endif