#include "testutil.h"

void write_bitmap(fuji_raw* raw, char* filename){
    FILE* img = fopen(filename, "wb");
    char header[512]; 
    // Header structure is: filetype width height max_pixel_value
    sprintf(header, "P5 %d %d %d\n", raw->cfa_width, raw->cfa_height, UINT16_MAX); // 32767 is maximum 14 bit value 
    fwrite(header, strlen(header), 1 , img);

    byte* buf = malloc(raw->cfa_length); 
    read(buf, raw->raw_data, 0, raw->cfa_length);
    // Byte order for PGM is big endian
    fbe(buf, 2, raw->cfa_length / 2);
    fwrite(buf, raw->cfa_width * raw->cfa_height * 2, 1 , img);
    fclose(img);
    free(buf);
}

void write_jpeg(fuji_raw* raw, char* filename){
    FILE* jpg = fopen(filename, "wb");
    fwrite(raw->jpeg_preview, raw->jpeg_img_length, 1, jpg);
    fclose(jpg);
}

void write_raw_tags(fuji_raw* raw, char* filename){
    FILE* f = fopen(filename, "w");
    for(int i = 0; i < raw->cfa_rec_count; i++){
        fprintf(f, "-----------\n");
        fprintf(f, "TagID: %04X\n", raw->cfa_rec[i].tag_id);
        fprintf(f, "Data:\n");
        for(int x = 0; x < raw->cfa_rec[i].size; x++){ 
            fprintf(f, "%02X", raw->cfa_rec[i].data[x]);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "CFA length: %d\n", raw->cfa_length);
    fprintf(f, "CFA width: %d\n", raw->cfa_width);
    fprintf(f, "CFA height: %d\n", raw->cfa_height);
    fclose(f);
}