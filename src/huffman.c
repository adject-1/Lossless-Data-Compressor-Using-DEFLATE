#include "huffman.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void count_frequencies(const uint8_t *data, int size, uint32_t *freq){
    memset(freq, 0, MAX_SYMBOLS * sizeof(uint32_t));
    for (int i = 0; i < size; i++) {
        freq[data[i]]++;
    }
}

HuffNode *heap[MAX_SYMBOLS];
int heap_size = 0;

void heap_push(HuffNode *node){
    int i = heap_size++;
    heap[i]= node;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent]->freq <= heap[i]->freq) {
            break;
        }
        HuffNode *tmp = heap[parent];
        heap[parent] = heap[i];
        heap[i] = tmp;

        i = parent;
    }
}

HuffNode *heap_pop(){
    HuffNode *top = heap[0];
    heap[0] = heap[--heap_size];
    int i = 0;
    while (1) {
        int left = 2*i+1;
        int right = 2*i+2;
        int smallest = i;
        if (left < heap_size && heap[left]->freq < heap[smallest]->freq) {
            smallest = left;
        }
        if (right < heap_size && heap[right]->freq < heap[smallest]->freq) {
            smallest = right;
        }
        if (smallest == i) break;
        HuffNode *tmp = heap[smallest];
        heap[smallest] = heap[i];
        heap[i] = tmp;

        i = smallest;
    }
    return top;
}

HuffNode *build_tree(uint32_t *freq){
    heap_size = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] == 0) continue;
        HuffNode *n = malloc(sizeof(HuffNode));
        n->freq = freq[i];
        n->symbol = i;
        n->left = n->right = NULL;
        heap_push(n);
    }
    while (heap_size > 1) {     // keep combining until only root is left(one node left)
        HuffNode *a = heap_pop();
        HuffNode *b = heap_pop();
        HuffNode *merged = malloc(sizeof(HuffNode));
        merged->freq = a->freq + b->freq;
        merged->symbol = 0;
        merged->left = a;
        merged->right = b;
        heap_push(merged);
    }
    return heap_pop();
}

uint8_t codes[MAX_SYMBOLS][MAX_SYMBOLS];
int code_lengths[MAX_SYMBOLS];

void generate_codes(HuffNode *node, uint8_t *code, int depth){
    if (!node->left && !node->right) {
        memcpy(codes[node->symbol], code, depth);
        code_lengths[node->symbol] = depth;
        return;
    }
    code[depth] = 0;
    generate_codes(node->left, code, depth+1);
    code[depth] = 1;
    generate_codes(node->right, code, depth+1);
}

void write_bit(BitWriter *bw, uint8_t bit){
    bw->buffer = (bw->buffer << 1) | bit;
    if (++bw->bit_count == 8) {
        fwrite(&bw->buffer, 1, 1, bw->file);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

void flush_bits(BitWriter *bw){
    if (bw->bit_count > 0) {
        bw->buffer <<= (8 - bw->bit_count);
        fwrite(&bw->buffer, 1, 1, bw->file);
    }
}

uint8_t read_bit (BitReader *br){
    if (br->bits_left == 0) {
        fread(&br->buffer, 1, 1, br->file);
        br->bits_left = 8;
    }
    br->bits_left--;
    return (br->buffer >> br->bits_left) & 1;
}
