/********************************************************************
 *
 *                          readwritecompressed.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Implementation for readwritecompressed.c
 *
 *     Summary:
 *      readwritecompressed either writes a UArray2b of 32-bit codewords to 
 *      stdout or reads in a compressed image from either a file or stdin and 
 *      converts the image into a UArray2b of 32-bit codewords
 * 
 *     Notes:
 *   - This module uses function from these other modules: uarray2b.h, 
 *     and bitpack.h
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "uarray2b.h"
#include "bitpack.h"

/**********print_to_stdout********
 *
 * Writes a compressed binary image to output in the appropriate format. Each 
 * 32-bit code word is written in big-endian order, and the 32-bit code words
 * are printed to output in row-major order.
 * Inputs:
 *              UArray2b_T compressed_blocks: the UArray2b that stores the 
 *                                            32-bit code words that correspond
 *                                            to each 2x2 block
 * Return: N/A
 * Expects:
 *      * compressed_blocks to be nonnull
 * Notes:
 *      * frees up memory for the inputted UArray2b
 *      * checked runtime error if:
 *              * compressed_blocks is NULL
 ************************/
void print_to_stdout(UArray2b_T compressed_blocks) 
{
        assert(compressed_blocks != NULL);
        unsigned width = UArray2b_width(compressed_blocks) * 2;
        unsigned height = UArray2b_height(compressed_blocks) * 2;

        printf("COMP40 Compressed image format 2\n%u %u", width, height);
        printf("\n");
        
        for (int row = 0; row < UArray2b_height(compressed_blocks); row++) {
                for (int col = 0; col < UArray2b_width(compressed_blocks); 
                                                                     col++) {
                        uint64_t current_word = 
                                *(uint64_t *)UArray2b_at(compressed_blocks, 
                                                                       col, 
                                                                       row);
                        /* prints out each word to stdout in big-endian order */
                        for (int i = 24; i >= 0; i -= 8) {
                                putchar(Bitpack_getu(current_word, 8, i));
                        }
                }
        }
        UArray2b_free(&compressed_blocks);
}

/**********read_compressed_file********
 *
 * Reads a compressed binary image from output in the appropriate format 
 * (ie reads the 32-bit code words in sequence). Stores the inputted 32-bit 
 * code words in a new UArray2b.
 * Inputs:
 *              FILE *input: a pointer to the input compressed binary image
 *                           file
 * Return: a UArray2b that holds the extracted 32-bit code words
 * Expects:
 *      * input to be nonnull
 *      * supplied input file to match the number of code words for the stated 
 *      width and height
 * Notes:
 *      * frees up memory for the inputted UArray2b
 *      * checked runtime error if:
 *              * input is NULL
 *              * if supplied file is too short for given width and height
 ************************/
UArray2b_T read_compressed_file(FILE *input)
{

        assert(input != NULL);
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                                           &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        UArray2b_T compressed_blocks = UArray2b_new(width / 2, height / 2, 
                                                        sizeof(uint64_t), 1);

        for (int row = 0; row < UArray2b_height(compressed_blocks); row++) {
                for (int col = 0; col < UArray2b_width(compressed_blocks); 
                                                                     col++) {
                        uint64_t current_word = 0;
                        /* reads in each word from input in big-endian order */
                        for (int i = 24; i >= 0; i -= 8) {
                                int current_bit = getc(input);
                                assert(current_bit != EOF);
                                current_word = Bitpack_newu(current_word, 8, i,
                                                                   current_bit);
                        }
                        *(uint64_t *)UArray2b_at(compressed_blocks, col, row) = 
                                                                   current_word;
                }
        }
        return compressed_blocks;        
}
