#include "raw.h"

void parse_raw(byte* buf, fuji_raw* raw){
    size_t cur_offset = 0;

    // Empirical values, found out by experimentation. Only holds true for X-T1 (and maybe other 1st gen Xtrans)
    // Make this configurable per camera
    raw->xtrans_offset_x = 2;
    raw->xtrans_offset_y = 1;

    raw->max_px_value = 16383; // Empirical value. Will change per camera

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
    // At least for Fuji X-T1 raw files, the difference between the cfa_length and the actual raw size is empty space at the beginning.
    // For the X-T1 it's 2048 bytes
    size_t raw_length = raw->cfa_width * raw->cfa_height * 2;
    raw->cfa_offset += raw->cfa_length - raw_length;
    raw->cfa_length = raw_length;

    read(raw->raw_data, buf, raw->cfa_offset, raw->cfa_length);
    // raw picture data is a bunch of 16 bit values (even though bitdepth is 14) 
    fbe(raw->raw_data, 2, raw->cfa_length / 2); 
}