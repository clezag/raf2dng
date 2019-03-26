#include "endian.h"

#define FR_BIG_ENDIAN 1
#define FR_LITTLE_ENDIAN 0
static uint8_t endianness = 2;

uint8_t determine_endianness(){ 
        uint16_t num = 0x1;
        uint8_t* ptr = (uint8_t*) &num;
        return ptr[0] ? FR_LITTLE_ENDIAN : FR_BIG_ENDIAN;
}

void flip_endian(void* p_buf, size_t size){
    byte* buf = p_buf;
    byte tmp;
    for(size_t i = 0; i < size/2; i++){
        tmp = buf[i];
        buf[i] = buf[size-1-i];
        buf[size-1-i] = tmp;
    } 
}

// from big endian
void fbe(void* p_buf, size_t size){
    if(endianness == 2){
       endianness = determine_endianness(); 
    }
    if(endianness == FR_LITTLE_ENDIAN){
        flip_endian(p_buf, size);
    } 
}
