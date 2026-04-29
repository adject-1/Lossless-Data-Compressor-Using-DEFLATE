#include "compress.h"
#include "huffman.h"
#include "intermediateSteps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress(FILE *in, FILE *out) {
    // 1. read entire input
    fseek(in, 0, SEEK_END);
    int size = ftell(in);
    rewind(in);
    uint8_t *data = malloc(size);
    fread(data, 1, size, in);

    // 2. lz77 encode
    LZ77Token *tokens = malloc(size * sizeof(LZ77Token));
    int token_count;
    lz77_encode(data, size, tokens, &token_count);
    LZ77Token *tokens_backup = malloc(token_count * sizeof(LZ77Token));
    memcpy(tokens_backup, tokens, token_count * sizeof(LZ77Token));
    free(data);

    // 3. flatten tokens into byte array for huffman
    int tsize = token_count * 3;
    uint8_t *tbytes = malloc(tsize);
    for (int i = 0; i < token_count; i++) {
        tbytes[i*3+0] = tokens[i].offset;
        tbytes[i*3+1] = tokens[i].length;
        tbytes[i*3+2] = tokens[i].next;
    }
    free(tokens);

    // 4. count frequencies and build huffman tree
    uint32_t freq[MAX_SYMBOLS];
    count_frequencies(tbytes, tsize, freq);
    HuffNode *root = build_tree(freq);
    uint8_t code[MAX_SYMBOLS];
    generate_codes(root, code, 0);

    // 5. write header: frequencies so decoder can rebuild tree
    fwrite(freq, sizeof(uint32_t), MAX_SYMBOLS, out);

    // 6. write token count so decoder knows when to stop
    fwrite(&token_count, sizeof(int), 1, out);

    // 7. huffman encode and write bits
    BitWriter bw = {0, 0, out};
    for (int i = 0; i < tsize; i++) {
        uint8_t sym = tbytes[i];
        for (int j = 0; j < code_lengths[sym]; j++)
            write_bit(&bw, codes[sym][j]);
    }

    flush_bits(&bw);
    print_pipeline(freq, tokens_backup, token_count, size, ftell(out), root);
    free(tokens_backup); // ← here, after the print
    free(tbytes);
}


void decompress(FILE *in, FILE *out) {
    uint32_t freq[MAX_SYMBOLS];
    //read header
    fread(freq, sizeof(uint32_t), MAX_SYMBOLS, in);
    HuffNode *root = build_tree(freq);

    int token_count;
    // fread(&token_count, sizeof(int), 1, in);
    if (fread(&token_count, sizeof(int), 1, in) != 1) {
        fprintf(stderr, "Bad compressed file\n");
        return;
    }
//decode huffman bitstream
    int tsize = token_count * 3;
    uint8_t *tbytes = malloc(tsize);
    BitReader br = {0, 0, in};
    for (int i = 0; i < tsize; i++) {
        HuffNode *node = root;
        while (node->left && node->right)
            node = read_bit(&br) ? node->right : node->left;
        tbytes[i] = node->symbol;
    }

    LZ77Token *tokens = malloc(token_count * sizeof(LZ77Token));
    for (int i = 0; i < token_count; i++) {
        tokens[i].offset = tbytes[i*3+0];
        tokens[i].length = tbytes[i*3+1];
        tokens[i].next   = tbytes[i*3+2];
    }
    free(tbytes);          // ← free tbytes here

    uint8_t *output = NULL;
    int out_size;
    lz77_decode(tokens, token_count, &output, &out_size);
    free(tokens);          // ← free tokens ONCE here only

    fwrite(output, 1, out_size, out);

    fseek(in,  0, SEEK_END); long compressed_size = ftell(in);
    print_decompress_pipeline(token_count, compressed_size, out_size);

    free(output);
}
