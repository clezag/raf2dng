#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "raw.h"
#include "testutil.h"

int main(int argc, char** argv){
    char* filename = argv[1];
    byte* buf;
    readFileToBuffer(&buf, filename);

    fuji_raw* raw = malloc(sizeof *raw);
    memset(raw, 0, sizeof *raw);
    parse_raw(buf, raw); 
    free(buf);

    write_raw_tags(raw, "test/tags.txt");
    write_jpeg(raw, "test/preview.jpg");
    write_bitmap(raw, "test/raw.pgm");

    return 0;
}