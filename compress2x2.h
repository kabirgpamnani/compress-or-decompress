/********************************************************************
 *
 *                          compress2x2.h
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *     * Interface for compress2x2.c
 *
 *     Summary:
 *      compress2x2 either compresses a 2D blocked array that holds component 
 *      video color space values into 32-bit code words, and stores the result
 *      in a new 2D blocked array, or decompresses a 2D blocked array of 32-bit 
 *      words into component video color space values and stores the resulting
 *      values in a new 2D blocked array. 
 *******************************************************************/
#ifndef COMPRESS2X2_INCLUDED
#define COMPRESS2X2_INCLUDED

UArray2b_T compressed2x2s(UArray2b_T componentUArray2b);
UArray2b_T decompressed2x2s(UArray2b_T compressed_blocks);
void decompress_one_block(int col, int row, UArray2b_T compressed_blocks, 
                                        void *elem, void *componentUArray2b);

uint64_t compress_one_block(UArray2b_T componentUArray2b, int col, int row);
CVS CVS_populator(float y, float pbavg, float pravg);

#endif