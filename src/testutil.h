#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
#include "raw.h"

// Write raw pixel data to PGM file
void write_bitmap(fuji_raw* raw, char* filename);

// Write included preview JPEG to file
void write_jpeg(fuji_raw* raw, char* filename);

// Write fuji raw tags (not JPEG EXIF!) to file
void write_raw_tags(fuji_raw* raw, char* filename);

#endif