/********************************************************************
 *
 *                          quantization.h
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Interface for quantization.c
 *
 *     Summary:
 *      quantization includes functions neccesary to perform quantization from 
 *      floats to scaled ints and dequantization from scaled intos to floats.
 *      The floats are to be stored in a struct of six float values, and the 
 *      scaled ints are to be stored in a 32-bit word using the bitpack.h 
 *      interface
 * 
*******************************************************************/

#ifndef QUANTIZATION_INCLUDED
#define QUANTIZATION_INCLUDED

typedef struct block_values *block_values;

/*
 * This is the struct definition of the block_values instance
 * Elements:
 *      float pbavg: a float value pbavg which represents the average of the 
 *                   pb values in each pixel in a 2x2 block
 *      float pravg: a float value pravg which represents the average of the 
 *                   pr values in each pixel in a 2x2 block
 *      float a: a float value a holding the average brightness of the image
 *      float b: a float value b which represents the 
 *               degree to which the image gets brighter as we move 
 *               from top to bottom
 *      float c: a float value c which represents the degree to which the 
 *               image gets brighter as we move from left to right
 *      float d: a float value d which represents the degree to which the 
 *               pixels on one diagonal are brighter than the pixels on the 
 *               other diagonal.
 *              
 */
struct block_values {
        float pbavg;
        float pravg;
        float a;
        float b;
        float c;
        float d;
};

uint64_t quantization(block_values one_block);
uint64_t packed_32_bit_word(unsigned pbavg, unsigned pravg, 
                                        unsigned a, int b, int c, int d);
int quantized_5bit(float value);

block_values unpacked_floats(uint64_t word);
float unquantized_5bit(int64_t five_bit);

#endif

