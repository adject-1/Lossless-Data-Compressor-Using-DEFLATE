#ifndef LZ77_H
#define LZ77_H

#include <stdint.h>
#include <stdio.h>

#define WINDOW_SIZE 255
#define MAX_MATCH 255

typedef struct {
    uint8_t offset;
    uint8_t length;
    uint8_t next;
} LZ77Token;

void lz77_encode(const uint8_t *data, int size, LZ77Token *tokens, int *token_count);
void lz77_decode(const LZ77Token *tokens, int token_count, uint8_t **output, int *out_size);

#endif
