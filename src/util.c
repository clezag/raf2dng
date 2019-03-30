#include "util.h"

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