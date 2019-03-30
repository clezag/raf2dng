#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stdio.h>

typedef uint8_t byte;

size_t read(void* tar, byte* src, size_t offset, size_t len);

void readFileToBuffer(byte** buf, char* filename);

#endif