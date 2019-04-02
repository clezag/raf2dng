#ifndef RAW_H
#define RAW_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "endian.h"

typedef struct {
    uint16_t tag_id;
    uint16_t size;
    byte* data;
} cfa_record;

typedef struct {
    // File header
    byte magic[16];
    char camera_str[32];
    byte version[4]; 
    // Offset directory
    uint32_t jpeg_img_offset;
    uint32_t jpeg_img_length;
    uint32_t cfa_header_offset;
    uint32_t cfa_header_length;
    uint32_t cfa_offset;
    uint32_t cfa_length; 
    // Preview jpeg
    byte* jpeg_preview;
    // CFA headers
    uint32_t cfa_rec_count;
    cfa_record* cfa_rec;
    // Actual raw data
    uint16_t cfa_height;
    uint16_t cfa_width;
    byte* raw_data;

    // Offset with which to start applying the color filter array
    uint8_t xtrans_offset_x;
    uint8_t xtrans_offset_y;

    // Maximum value of a single raw data point
    uint16_t max_px_value;
} fuji_raw;

typedef enum {R = 0,
              G = 1,
              B = 2} color;
static const color xtrans_cfa[6][6] = {{G,B,G,G,R,G},
                                       {R,G,R,B,G,B},
                                       {G,B,G,G,R,G},
                                       {G,R,G,G,B,G},
                                       {B,G,B,R,G,R},
                                       {G,R,G,G,B,G}};

void parse_raw(byte* buf, fuji_raw* raw);

#endif