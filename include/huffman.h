#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>

#define MAX_SYMBOLS 256

// Node structure of Huffman Tree
typedef struct HuffNode{
    uint32_t freq;
    uint8_t symbol;
    struct HuffNode *left, *right;
} HuffNode;

typedef struct {
    uint8_t buffer; // the current bit being built
    int bit_count;
    FILE *file;
} BitWriter;

typedef struct{
    uint8_t buffer;
    int bits_left;
    FILE *file;
} BitReader;


void count_frequencies(const uint8_t *data, int size, uint32_t *freq);
HuffNode *build_tree(uint32_t *freq);
void generate_codes(HuffNode *node, uint8_t *code, int depth);
void write_bit(BitWriter *bw, uint8_t bit);
void flush_bits(BitWriter *bw);
uint8_t read_bit(BitReader *br);

extern uint8_t codes[MAX_SYMBOLS][MAX_SYMBOLS];
extern int code_lengths[MAX_SYMBOLS];
extern int heap_size;

#endif
