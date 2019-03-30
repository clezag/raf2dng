#include "endian.h"

#define FR_BIG_ENDIAN 1
#define FR_LITTLE_ENDIAN 0
static uint8_t endianness = 2;

uint8_t determine_endianness(){ 
        uint16_t num = 0x1;
        uint8_t* ptr = (uint8_t*) &num;
        return ptr[0] ? FR_LITTLE_ENDIAN : FR_BIG_ENDIAN;
}

void flip_endian(void* p_buf, size_t size, size_t blocks){
    byte* buf = p_buf;
    byte tmp;
    size_t halfsize = size/2;
    for(size_t b = 0; b < blocks; b++){
        size_t offset = b * size;
        for(size_t i = 0; i < halfsize; i++){
            tmp = buf[offset + i];
            buf[offset + i] = buf[offset + size-1-i];
            buf[offset + size-1-i] = tmp;
        } 
    }
}

void fbe(void* p_buf, size_t size, size_t blocks){
    if(endianness == 2){
       endianness = determine_endianness(); 
    }
    if(endianness == FR_LITTLE_ENDIAN){
        flip_endian(p_buf, size, blocks);
    } 
}
