#include "testutil.h"

void write_greyscale_bitmap(fuji_raw* raw, char* filename){
    FILE* img = fopen(filename, "wb");
    char header[512]; 
    // Header structure is: filetype width height max_pixel_value
    sprintf(header, "P5 %d %d %d\n", raw->cfa_width, raw->cfa_height, 32767); // 32767 is maximum 14 bit value 
    fwrite(header, strlen(header), 1 , img);

    byte* buf = malloc(raw->cfa_length); 
    read(buf, raw->raw_data, 0, raw->cfa_length);
    // Byte order for PGM is big endian
    fwrite(buf, raw->cfa_width * raw->cfa_height * 2, 1 , img);
    fclose(img);
    free(buf);
}

void write_color_bitmap(fuji_raw* raw, char* filename){
    FILE* img = fopen(filename, "wb");
    char header[512]; 
    // Header structure is: filetype width height max_pixel_value
    sprintf(header, "P6 %d %d %d\n", raw->cfa_width, raw->cfa_height, 32767); // 32767 is maximum 14 bit value 
    fwrite(header, strlen(header), 1 , img);

    size_t bufsize = raw->cfa_width * raw->cfa_height * 6; // three 16 bit values per pixel
    byte* buf = malloc(bufsize); 
    memset(buf, 0, bufsize);

    // Byte order for PPM is big endian
    size_t raw_pitch = raw->cfa_width * 2;
    size_t ppm_pitch = raw->cfa_width * 6;
    for(size_t y = 0; y < raw->cfa_height; y++){
        for(size_t x = 0; x < raw->cfa_width; x++){
            color c = xtrans_cfa[y % 6][x % 6]; // determine which color the pixel is, applying the color filter array
            buf[y * ppm_pitch + x * 6 + 2 * c] = *(raw->raw_data + y * raw_pitch + x * 2);
        }
    }

    fwrite(buf, bufsize, 1 , img);
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