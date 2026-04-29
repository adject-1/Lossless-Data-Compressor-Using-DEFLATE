#include "lz77.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lz77_encode(const uint8_t *data, int size,
                 LZ77Token *tokens, int *token_count)
{
    int pos = 0;
    *token_count = 0;

    while (pos < size) {
        int best_offset = 0;
        int best_length = 0;

        int window_start = (pos > WINDOW_SIZE) ? pos - WINDOW_SIZE : 0;

        for (int i = window_start; i < pos; i++) {
            int length = 0;

            while (
                pos + length < size &&
                i + length < pos &&          // IMPORTANT FIX
                data[i + length] == data[pos + length] &&
                length < MAX_MATCH
            ) {
                length++;
            }

            if (length > best_length) {
                best_length = length;
                best_offset = pos - i;
            }
        }

        tokens[*token_count].offset = best_offset;
        tokens[*token_count].length = best_length;
        tokens[*token_count].next =
            (pos + best_length < size) ? data[pos + best_length] : 0;

        (*token_count)++;

        pos += best_length + 1;
    }
}

void lz77_decode(const LZ77Token *tokens, int token_count,
                 uint8_t **output, int *out_size)
{
    int capacity = 1024 * 1024;   // 1 MB start
    uint8_t *buf = malloc(capacity);
    int pos = 0;

    for (int i = 0; i < token_count; i++) {
        int offset = tokens[i].offset;
        int length = tokens[i].length;
        uint8_t next = tokens[i].next;

        while (pos + length + 1 >= capacity) {
            capacity *= 2;
            buf = realloc(buf, capacity);
        }

        for (int j = 0; j < length; j++) {
            buf[pos] = buf[pos - offset];
            pos++;
        }

        if (i < token_count - 1 || next != 0)
            buf[pos++] = next;
    }

    *output = buf;
    *out_size = pos;
}
