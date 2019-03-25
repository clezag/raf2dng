#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "type.h"
#include "endian.h"

typedef struct {
    byte tag_id[2];
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
    byte* raw_data;
} fuji_raw;

size_t read(void* tar, byte* src, size_t offset, size_t len){ 
    memcpy((char*) tar, src + offset, len);
    return len;
} 

void readFileToBuffer(byte** buf, char* filename){ 
    FILE* f = fopen(filename, "r");
    if(!f){
        perror("Could not open file\n");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    *buf = (byte*) malloc((len+1) * sizeof(byte));
    if(!buf){
        perror("unable to allocate file buffer");
        exit(1);
    }
    size_t bytes_read = fread(*buf, sizeof(byte), len, f);
    if(bytes_read != len){ 
        if(feof(f)){
            printf("Unexpected EOF reached\n");
        } else{
            perror("Unable to read file");
            printf("Read %d bytes instead of expected %d\n", (int) bytes_read, (int) len);
        }
        exit(1);
    }
    fclose(f);
}

void parse_raw(byte* buf, fuji_raw* raw){
    size_t cur_offset = 0;


    cur_offset += read(raw->magic, buf, cur_offset, 16);
    cur_offset += 12; // unknown bytes
    cur_offset += read(raw->camera_str, buf, cur_offset, 32);
    cur_offset += read(raw->version, buf, cur_offset, 4);
    cur_offset += 20; // unknown bytes
    cur_offset += read(&raw->jpeg_img_offset, buf, cur_offset, 4);
    cur_offset += read(&raw->jpeg_img_length, buf, cur_offset, 4);
    cur_offset += read(&raw->cfa_header_offset, buf, cur_offset, 4);
    cur_offset += read(&raw->cfa_header_length, buf, cur_offset, 4);
    cur_offset += read(&raw->cfa_offset, buf, cur_offset, 4);
    cur_offset += read(&raw->cfa_length, buf, cur_offset, 4);

    FBE(raw->jpeg_img_offset);
    FBE(raw->jpeg_img_length);
    FBE(raw->cfa_header_offset);
    FBE(raw->cfa_header_length);
    FBE(raw->cfa_offset);
    FBE(raw->cfa_length);

    // deal with endianness. We assume that we are LE, the file is BE

    // // File header
    // char magic[16];
    // char camera_str[32];
    // byte version[4]; 
    // // Offset directory
    // uint32_t jpeg_img_offset;
    // uint32_t jpeg_img_length;
    // uint32_t cfa_header_offset;
    // uint32_t cfa_header_length;
    // uint32_t cfa_offset;
    // uint32_t cfa_length; 
    // // Preview jpeg
    // byte* jpeg_preview;
    // // CFA headers
    // uint32_t cfa_rec_count;
    // cfa_record* cfa_rec;
    // // Actual raw data
    // byte* raw_data;
}

int main(int argc, char** argv){
    char* filename = argv[1];
    byte* buf;
    readFileToBuffer(&buf, filename);

    fuji_raw* raw = malloc(sizeof *raw);
    memset(raw, 0, sizeof *raw);
    parse_raw(buf, raw);

    free(buf);

    return 0;
}