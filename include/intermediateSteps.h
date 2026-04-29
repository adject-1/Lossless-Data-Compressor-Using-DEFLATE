#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "huffman.h"
#include "lz77.h"
#include <stdio.h>
#include <string.h>

static void print_tree(HuffNode *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth * 3; i++) printf(" ");
    if (!node->left && !node->right)
        printf("└─ [%d] (%u)\n", node->symbol, node->freq);
    else
        printf("└─ (*) freq=%u\n", node->freq);
    print_tree(node->left,  depth + 1);
    print_tree(node->right, depth + 1);
}

static void print_pipeline(uint32_t freq[MAX_SYMBOLS], LZ77Token *tokens,
                            int token_count, long original_size, long compressed_size, HuffNode *root) {

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║         COMPRESSION PIPELINE             ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    /* Stage 1: Raw input */
    printf("\n  [1] RAW INPUT\n");
    printf("      %ld bytes\n", original_size);

    /* Stage 2: LZ77 tokens */
    printf("\n       ↓  lz77 encode\n\n");
    printf("  [2] LZ77 TOKENS: %d tokens generated\n", token_count);
    int show = token_count < 5 ? token_count : 5;
    for (int i = 0; i < show; i++)
        printf("      [%d] offset=%-4d len=%-4d next='%c'\n",
               i, tokens[i].offset, tokens[i].length,
               (tokens[i].next >= 32 && tokens[i].next <= 126)
               ? tokens[i].next : '?');
    if (token_count > 5) printf("      ... and %d more\n", token_count - 5);

    /* Stage 3: Huffman frequencies */
    printf("\n       ↓  flatten tokens → count frequencies\n\n");
    printf("  [3] TOP 5 BYTE FREQUENCIES:\n");
    uint32_t tmp[MAX_SYMBOLS];
    memcpy(tmp, freq, sizeof(tmp));
    for (int n = 0; n < 5; n++) {
        uint32_t max = 0; int idx = -1;
        for (int i = 0; i < MAX_SYMBOLS; i++)
            if (tmp[i] > max) { max = tmp[i]; idx = i; }
        if (idx == -1 || max == 0) break;
        printf("      byte=%-4d freq=%u\n", idx, max);
        tmp[idx] = 0;
    }

    /* Stage 4: Print Huffmann Tree */
    printf("\n       ↓  huffman tree\n\n");
    printf("  [4] HUFFMAN TREE:\n");
    print_tree(root, 2);

    /* Stage 5: Result */
    printf("\n       ↓  huffman encode\n\n");
    printf("  [4] COMPRESSED OUTPUT\n");
    printf("      %ld bytes (%.1f%% smaller)\n",
           compressed_size,
           100.0 * (1.0 - (double)compressed_size / original_size));

}

static void print_decompress_pipeline(int token_count, long compressed_size, long output_size) {

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║       DECOMPRESSION PIPELINE             ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    /* Stage 1: Compressed input */
    printf("\n  [1] COMPRESSED FILE\n");
    printf("      %ld bytes\n", compressed_size);

    /* Stage 2: Huffman decode */
    printf("\n       ↓  read header → rebuild huffman tree\n\n");
    printf("  [2] HUFFMAN DECODE\n");
    printf("      %d tokens recovered from bitstream\n", token_count);

    /* Stage 3: Unflatten tokens */
    printf("\n       ↓  unflatten bytes → tokens\n\n");
    printf("  [3] LZ77 TOKENS RESTORED\n");
    printf("      %d tokens  (%d bytes each = %d bytes total)\n",
           token_count, 3, token_count * 3);

    /* Stage 4: LZ77 decode */
    printf("\n       ↓  lz77 decode → expand back-references\n\n");
    printf("  [4] ORIGINAL FILE RESTORED\n");
    printf("      %ld bytes\n", output_size);
    printf("      Expansion: %.1fx original size\n",
           (double)output_size / compressed_size);

    printf("\n  FILE ──→ HUFFMAN ──→ TOKENS ──→ LZ77 ──→ RAW\n");
    printf("  %4ld B     decode      restore    expand   %4ld B\n\n",
           compressed_size, output_size);
}

#endif
