#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "type.h"
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

    // Read header and offset directory
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

    // deal with endianness. We assume that we are LE, the file is BE
    FBE(raw->jpeg_img_offset);
    FBE(raw->jpeg_img_length);
    FBE(raw->cfa_header_offset);
    FBE(raw->cfa_header_length);
    FBE(raw->cfa_offset);
    FBE(raw->cfa_length);

    // Read preview JPG
    raw->jpeg_preview = malloc(raw->jpeg_img_length); 
    if(!raw->jpeg_preview){
        perror("Unable to allocate memory for preview jpeg");
        exit(1); 
    }
    read(raw->jpeg_preview, buf, raw->jpeg_img_offset, raw->jpeg_img_length);

    // Read CFA headers (Tags)
    cur_offset = raw->cfa_header_offset;
    cur_offset += read(&raw->cfa_rec_count, buf, cur_offset, 4);
    FBE(raw->cfa_rec_count);

    raw->cfa_rec = malloc(raw->cfa_rec_count * sizeof(cfa_record));
    if(!raw->cfa_rec){
        perror("Unable to allocate memory for CFA records");
        exit(1); 
    }
    memset(raw->cfa_rec, 0, raw->cfa_rec_count * sizeof(cfa_record));

    size_t cfa_rec_counter = 0;
    for(int i = 0; i < raw->cfa_rec_count; i++){ 
        cfa_record* rec = &raw->cfa_rec[cfa_rec_counter];
        cur_offset += read(&rec->tag_id, buf, cur_offset, sizeof(rec->tag_id));
        FBE(rec->tag_id);
        cur_offset += read(&rec->size, buf, cur_offset, sizeof(rec->size));
        FBE(rec->size);
        rec->data = malloc(rec->size);
        if(!rec->data){
            perror("Unable to allocate memory for cfa header record");
            exit(1);
        }
        cur_offset += read(rec->data, buf, cur_offset, rec->size);

        // Tag ID  0x0100 is the full image size, with two bytes for height and width, respectively
        if(rec->tag_id == 0x0100){
            raw->cfa_height = ((uint16_t*)rec->data)[0];
            FBE(raw->cfa_height);
            raw->cfa_width = ((uint16_t*)rec->data)[1]; 
            FBE(raw->cfa_width);
        }
        cfa_rec_counter++;
    }
    
    // Read actual raw data
    raw->raw_data = malloc(raw->cfa_length); 
    if(!raw->raw_data){
        perror("Unable to allocate memory for raw data");
        exit(1); 
    }
    read(raw->raw_data, buf, raw->cfa_offset, raw->cfa_length);
    // raw picture data is a bunch of 16 bit values (even though bitdepth is 14) 
    //fbe(raw->raw_data, 2, raw->cfa_length/2); 
}

void write_bitmap(fuji_raw* raw, char* filename){
    FILE* img = fopen(filename, "wb");
    char header[512]; 
    sprintf(header, "P5 %d %d %d\n", raw->cfa_width, raw->cfa_height, UINT16_MAX); // 32767 is maximum 14 bit value 
    fwrite(header, strlen(header), 1 , img);

    byte* ptr = raw->raw_data + 2048; // Apparently, the difference between cfa_length and 16/8 * height * length is an empty bit at the beginning of the raw data
    fwrite(ptr, raw->cfa_width * raw->cfa_height * 2, 1 , img);
    fclose(img);
}


int main(int argc, char** argv){
    char* filename = argv[1];
    byte* buf;
    readFileToBuffer(&buf, filename);

    fuji_raw* raw = malloc(sizeof *raw);
    memset(raw, 0, sizeof *raw);
    parse_raw(buf, raw); 
    free(buf);

    // FILE* jpg = fopen("test.jpg", "wb");
    // fwrite(raw->jpeg_preview, raw->jpeg_img_length, 1, jpg);
    // fclose(jpg);

    // for(int i = 0; i < raw->cfa_rec_count; i++){
    //     printf("-----------\n");
    //     printf("TagID: %04X\n", raw->cfa_rec[i].tag_id);
    //     printf("Data:\n");
    //     for(int x = 0; x < raw->cfa_rec[i].size; x++){ 
    //         printf("%02X", raw->cfa_rec[i].data[x]);
    //     }
    //     printf("\n");
    // }
    printf("CFA length: %d\n", raw->cfa_length);
    printf("CFA width: %d\n", raw->cfa_width);
    printf("CFA height: %d\n", raw->cfa_height);

    write_bitmap(raw, "test.pgm");

    return 0;
}