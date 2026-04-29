#include <stdio.h>
#include <string.h>
#include "compress.h"
int main(int argc, char *argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [compress | decompress] <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[2], "rb");
    FILE *out = fopen(argv[3], "wb");

    if (!in || !out) {
        perror("File did not opened");
        return 1;
    }

    if (strcmp(argv[1], "compress") == 0) {
        compress(in, out);
    }
    else if (strcmp(argv[1], "decompress") == 0) {
        decompress(in, out);
    }
    else {
        fprintf(stderr, "Command not found %s", argv[1]);
        return 1;
    }

    fclose(in);
    fclose(out);
    return 0;
}
