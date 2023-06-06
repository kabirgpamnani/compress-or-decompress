/********************************************************************
 *
 *                          compress2x2.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Implementation for compress2x2.h
 *
 *     Summary:
 *      compress2x2 either compresses a 2D blocked array that holds component 
 *      video color space values into 32-bit code words, and stores the result
 *      in a new 2D blocked array, or decompresses a 2D blocked array of 32-bit 
 *      words into component video color space values and stores the resulting
 *      values in a new 2D blocked array. 
 * 
 *     Notes:
 *   - This module uses functions from these other modules: uarray2b.h, 
 *     pnm.h, rgbcomponent.h, bitpack.h, and quantization.h
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "pnm.h"
#include "uarray2b.h"
#include "rgbcomponent.h"
#include "bitpack.h"
#include "compress2x2.h"
#include "quantization.h"

#define COMPRESSED_BLOCK_SIZE 1

/**********compressed2x2s********
 *
 * Compresses a UArray2b that holds component video color space (CVS) structs 
 * into a UArray2b that holds 32-bit words. The 4 CVS structs in each 2x2 block
 * get converted into one 32-bit word.
 * Inputs:
 *              UArray2b_T componentUArray2b: the UArray2b of CVS structs
 * Return: A UArray2b of 32-bit words with each 2x2 block corresponding to one
 *         32 bit word
 * Expects:
 *      * componentUArray2b to be nonnull
 *      * frees up the memory used for the inputted UArray2b_T, and allocates 
 *        memory for the returned UArray2b_T. The caller assumes ownership of 
 *        the returned UArray2b_T
 * Notes:
 *      * Checked runtime error if:
 *              * componentUArray2b is NULL
 ************************/
UArray2b_T compressed2x2s(UArray2b_T componentUArray2b) 
{
        assert(componentUArray2b != NULL);

        int original_width = UArray2b_width(componentUArray2b);
        int original_height = UArray2b_height(componentUArray2b);
        
        int new_width = original_width / 2;
        int new_height = original_height / 2;

        UArray2b_T compressed_blocks = UArray2b_new(new_width, new_height, 
                                                        sizeof(uint64_t), 
                                                        COMPRESSED_BLOCK_SIZE);

        /* Converts each 2x2 block from four CVS structs to one 32-bit word */
        for (int row = 0; row < original_height; row += 2) {
                for (int col = 0; col < original_width; col += 2) {
                        uint64_t curr_32_bit_word = 
                                compress_one_block(componentUArray2b, col, row);
                        (*(uint64_t *)UArray2b_at(compressed_blocks, 
                                        col / 2, row / 2)) = curr_32_bit_word;
                }
        }

        UArray2b_free(&componentUArray2b);
        return compressed_blocks;
}

/**********compress_one_block********
 *
 * Converts the values in 4 component video color space (CVS) structs to one 32 
 * bit word and returns the 32 bit word that corresponds to each 2x2 block in
 * the UArray2b.
 * Inputs:
 *              UArray2b_T componentUArray2b: the UArray2b that stores the CVS
 *                                            struct values which are to be 
 *                                            converted and packed into a 32
 *                                            bit word
 *              int col: the column value of the current position of the
 *                       CVS struct in the UArray2b
 *              int row: the row value of the current position of the
 *                       CVS struct word in the UArray2b
 * Return: a 32 bit word that corresponds to each 2x2 block in the UArray2b
 * Expects:
 *      componentUArray2b to be nonnull
 *      one_block to be nonnull (created within function, not passed in)
 * Notes:
 *      * to be called on every 2x2 block in the UArray2b (called in function
 *      compressed 2x2s)
 *      * checked runtime error if
 *              * componentUArray2b is NULL
 *              * one_block is NULL
 ************************/
uint64_t compress_one_block(UArray2b_T componentUArray2b, int col, int row)
{
        assert(componentUArray2b != NULL);
        block_values one_block = malloc(sizeof(struct block_values));
        assert(one_block != NULL);

        /* start of the discrete cosine transform */
        float Y1 = ((CVS)UArray2b_at(componentUArray2b, col, row))->y;
        float Y2 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row))->y;
        float Y3 = ((CVS)UArray2b_at(componentUArray2b, col, row + 1))->y;
        float Y4 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row + 1))->y;

        one_block->a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        one_block->b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        one_block->c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        one_block->d = (Y4 - Y3 - Y2 + Y1) / 4.0;
        /* end of the discrete cosine transform */

        float pb1 = ((CVS)UArray2b_at(componentUArray2b, col, row))->pb;
        float pb2 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row))->pb;
        float pb3 = ((CVS)UArray2b_at(componentUArray2b, col, row + 1))->pb;
        float pb4 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row + 1))->pb;
                        
        float pr1 = ((CVS)UArray2b_at(componentUArray2b, col, row))->pr;
        float pr2 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row))->pr;
        float pr3 = ((CVS)UArray2b_at(componentUArray2b, col, row + 1))->pr;
        float pr4 = ((CVS)UArray2b_at(componentUArray2b, col + 1, row + 1))->pr;
                        
        /* averaging the pb and pr values for 4 pixels */
        one_block->pbavg = (pb1 + pb2 + pb3 + pb4) / 4.0;
        one_block->pravg = (pr1 + pr2 + pr3 + pr4) / 4.0;

        uint64_t one_32_bit_word = quantization(one_block);
        free(one_block);
        
        return one_32_bit_word;
}


/**********decompressed2x2s********
 *
 * Decompresses a UArray2b of 32-bit words into a UArray2b that hold component
 * video color space (CVS) structs. Each 32-bit word gets converted into 4 CVS 
 * structs in a 2x2 pixel block.
 * Inputs:
 *              UArray2b_T compressed_blocks: The 2d array of 32-bit words
 * Return: A UArray2b of CVS structs with each struct representing a pixel
 * Expects:
 *      * compressed_blocks to be non NULL
 *      * frees up the memory used for the inputted UArray2b_T, and allocates 
 *        memory for the returned UArray2b_T. The caller assumes ownership of 
 *        the returned UArray2b_T
 * Notes:
 *      * Checked runtime error if:
 *              * compressed_blocks is NULL
 ************************/
UArray2b_T decompressed2x2s(UArray2b_T compressed_blocks) 
{
        assert(compressed_blocks != NULL);

        int original_width = UArray2b_width(compressed_blocks);
        int original_height = UArray2b_height(compressed_blocks);
        
        int new_width = original_width * 2;
        int new_height = original_height * 2;

        UArray2b_T componentUArray2b = UArray2b_new(new_width, new_height, 
                                                        sizeof(struct CVS), 
                                                        COMPRESSED_BLOCK_SIZE);

        UArray2b_map(compressed_blocks, decompress_one_block, 
                                                        componentUArray2b);
        UArray2b_free(&compressed_blocks);

        return componentUArray2b;
}      

/**********decompress_one_block********
 *
 * Converts one 32-bit word to 4 component video color space (CVS) structs and
 * places each one in a 2x2 block in its corresponding decompressed location in
 * a new UArray2b (componentUArray2b)
 * Inputs:
 *              int col: the column value of the current position of the
 *                       32-bit word in the UArray2b
 *              int row: the row value of the current position of the
 *                       32-bit word in the UArray2b
 *              UArray2b_T compressed_blocks: a UArray2b storing the 32-bit
 *                       words
 *              void *elem (voided): a pointer to the element in 
 *                       compressed_blocks at position (col, row)
 *              void *componentUArray2b: UArray2b where the resulting 4 CVS
 *                       structs are put into
 * Return: N/A
 * Expects:
 *      componentUArray2b to be nonnull
 *      float_block_values to be nonnull (created within function, 
 *                                        not passed in)
 * Notes:
 *      * to be used as an apply function in the UArray2b_map function
 *      * checked runtime error if
 *              * componentUArray2b is NULL
 *              * float_block_values is NULL
 ************************/
void decompress_one_block(int col, int row, UArray2b_T compressed_blocks, 
                                        void *elem, void *componentUArray2b)
{
        (void)elem;
        assert(componentUArray2b != NULL);

        /* 
         * calls modules in quantization.h to turn a 32-bit word into a struct 
         * of 6 float values 
         */
        block_values float_block_values = 
                unpacked_floats(*(uint64_t *)UArray2b_at(compressed_blocks, 
                                                                          col, 
                                                                          row));
        assert(float_block_values != NULL);

        float a = float_block_values->a;
        float b = float_block_values->b;
        float c = float_block_values->c;
        float d = float_block_values->d;
        float pbavg = float_block_values->pbavg;
        float pravg = float_block_values->pravg;

        free(float_block_values);

        /* inverse discrete cosine transform */
        float Y1 = a - b - c + d;
        float Y2 = a - b + c - d;
        float Y3 = a + b - c - d;
        float Y4 = a + b + c + d;

        /* repopulate the decompressed UArray2 with the four values */

        CVS top_left = CVS_populator(Y1, pbavg, pravg);
        CVS top_right = CVS_populator(Y2, pbavg, pravg);
        CVS bottom_left = CVS_populator(Y3, pbavg, pravg);
        CVS bottom_right = CVS_populator(Y4, pbavg, pravg);
                        
        *(CVS)UArray2b_at(componentUArray2b, col * 2, row * 2) = *top_left;
        *(CVS)UArray2b_at(componentUArray2b, col * 2 + 1, row * 2) = *top_right;
        *(CVS)UArray2b_at(componentUArray2b, col * 2, row * 2 + 1) = 
                                                                *bottom_left;
        *(CVS)UArray2b_at(componentUArray2b, col * 2 + 1, row * 2 + 1) = 
                                                                *bottom_right;

        free(top_left);   
        free(top_right);   
        free(bottom_left);   
        free(bottom_right);


}

/**********CVS_populator********
 *
 * Populates a struct 
 * Inputs:
 *              FILE *input: pointer to the input PPM file
 * Return: N/A
 * Expects:
 *      * pointer to input PPM file to be nonnull
 * Notes:
 *      * Checked runtime error if:
 *              * pointer to input PPM file is NULL
 ************************/
CVS CVS_populator(float y, float pbavg, float pravg) 
{
        CVS one_pixel = malloc(sizeof(struct CVS));
        assert(one_pixel != NULL);

        one_pixel->y = y;
        one_pixel->pb = pbavg;
        one_pixel->pr = pravg;

        return one_pixel;
}

