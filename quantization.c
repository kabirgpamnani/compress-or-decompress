/********************************************************************
 *
 *                          quantization.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Implementation for quantization.h
 *
 *     Summary:
 *      quantization includes functions neccesary to perform quantization from 
 *      floats to scaled ints and dequantization from scaled intos to floats.
 *      The floats are to be stored in a struct of six float values, and the 
 *      scaled ints are to be stored in a 32-bit word using the bitpack.h 
 *      interface
 * 
 *     Notes:
 *   - This module uses function from these other modules: arith40.h, 
 *     and bitpack.h
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "assert.h"
#include "arith40.h"
#include "bitpack.h"
#include "quantization.h"

/* bit size values for each value being stored in the 32-bit word */
#define A_BIT_SIZE 6
#define B_BIT_SIZE 6
#define C_BIT_SIZE 6
#define D_BIT_SIZE 6
#define PB_BIT_SIZE 4
#define PR_BIT_SIZE 4

/* 
 * least significant bit values for each value being stored in the 32-bit word
 */
#define A_LSB 26
#define B_LSB 20
#define C_LSB 14
#define D_LSB 8
#define PB_LSB 4
#define PR_LSB 0

/* our specific literals for quantizing and dequantizing a, b, c, and d */
#define A_CODE 63.0
#define BCD_CODE (0.6 / 63)

/**********quantization********
 *
 * Quantizes 6 float values into scaled ints and returns a 32-bit word 
 * representing those values
 * Inputs:
 *              block_values one_block: A struct contraining 6 floats values
 *                                      to be inserted into the 32-bit word
 * Return: A 64-bit word holding the quantized 32-bit word with the 6 values
 * Expects:
 *      * one_block to be non NULL
 * Notes:
 *      * uses the arith40.h interface to find the index of pbavg and pravg
 *      * checked runtime error if:
 *              * one_block is NULL
 ************************/
uint64_t quantization(block_values one_block) 
{
        assert(one_block != NULL);

        unsigned pbavg_index = Arith40_index_of_chroma(one_block->pbavg);
        unsigned pravg_index = Arith40_index_of_chroma(one_block->pravg);
        unsigned a = round((one_block->a) * A_CODE);
        int b = quantized_5bit(one_block->b);
        int c = quantized_5bit(one_block->c);
        int d = quantized_5bit(one_block->d);

        uint64_t one_32_bit_word = packed_32_bit_word(pbavg_index, pravg_index, 
                                                                   a, b, c, d);

        return one_32_bit_word;
}

/**********packed_32_bit_word********
 *
 * Packs 6 quantized scaled int values into a 32-bit word
 * Inputs:
 *              unsigned pbavg: a quantized pb value to be packed
 *              unsigned pravg: a quantized pr value to be packed
 *                  unsigned a: a quantized a value to be packed
 *                       int b: a quantized b value to be packed
 *                       int c: a quantized c value to be packed
 *                       int d: a quantized d value to be packed
 * Return: A 64-bit word holding the packed 32-bit word with the 6 values
 * Expects:
 *      N/A
 * Notes:
 *      * uses the bitpack.h interface to pack the ints into their correct bits
 *        within the 32-bit word
 ************************/
uint64_t packed_32_bit_word(unsigned pbavg, unsigned pravg, unsigned a, int b,
                                                                        int c, 
                                                                        int d)
{
        uint64_t word = 0;

        word = Bitpack_newu(word, A_BIT_SIZE, A_LSB, a);
        word = Bitpack_news(word, B_BIT_SIZE, B_LSB, b);
        word = Bitpack_news(word, C_BIT_SIZE, C_LSB, c);
        word = Bitpack_news(word, D_BIT_SIZE, D_LSB, d);
        word = Bitpack_newu(word, PB_BIT_SIZE, PB_LSB, pbavg);
        word = Bitpack_newu(word, PR_BIT_SIZE, PR_LSB, pravg);

        return word;
}

/**********quantized_5bit********
 *
 * Quantizes a float value to a 5 bit int representation
 * Inputs:
 *              float value: the value to be quantized
 * Return: the 5 bit representation of value
 * Expects:
 *      N/A
 * Notes:
 *      * five_bit, the returned int, will be between the int values of 
 *        -15 and 15
 ************************/
int quantized_5bit(float value) 
{
        int five_bit;
        if (value <= 0.3 && value >= -0.3) {
                five_bit = round(value / BCD_CODE);
        } else if (value < -0.3) {
                five_bit = ((pow(2, B_BIT_SIZE)) / -2) + 1;
        } else if (value > 0.3) {
                five_bit = ((pow(2, B_BIT_SIZE)) / 2) - 1;
        }
        return five_bit;
}

/**********unpacked_floats********
 *
 * Unpacks and unquantizes a 32-bit word into 6 float values
 * Inputs:
 *              uint64_t word: a 64-bit word holding the 32-bit word being 
 *                             unpacked and unquantized
 * Return: a pointer to a struct containing 6 float values, representing the 6
 *         values from the compressed image 32-bit word unquantized into floats
 * Expects:
 *      N/A
 * Notes:
 *      * Allocates memory for the block_values struct that isn't freed within
 *        this function. Caller assumes ownership of the returned block_values
 *        struct.
 *      * uses the arith40.h interface to find the chroma of the pbavg and 
 *        pravg indices
 *      * uses the bitpack.h interface to find the ints from their specific
 *        bits within the 32-bit word inputted
 ************************/
block_values unpacked_floats(uint64_t word) 
{
        block_values float_one_block = malloc(sizeof(struct block_values));
        
        float_one_block->a = (Bitpack_getu(word, A_BIT_SIZE, A_LSB)) / A_CODE;
        float_one_block->b = unquantized_5bit(Bitpack_gets(word, B_BIT_SIZE, 
                                                                        B_LSB));
        float_one_block->c = unquantized_5bit(Bitpack_gets(word, C_BIT_SIZE, 
                                                                        C_LSB));
        float_one_block->d = unquantized_5bit(Bitpack_gets(word, D_BIT_SIZE,
                                                                        D_LSB));
        float_one_block->pbavg = Arith40_chroma_of_index
                                      (Bitpack_getu(word, PB_BIT_SIZE, PB_LSB));
        float_one_block->pravg = Arith40_chroma_of_index
                                      (Bitpack_getu(word, PR_BIT_SIZE, PR_LSB));

        return float_one_block;
}

/**********unquantized_5bit********
 *
 * Unquantizes a 5 bit representation of an int back to a float
 * Inputs:
 *              int64_t five_bit: the 5 bit representation of an int
 * Return: the unquantized float of five_bit
 * Expects:
 *      * five_bit to be between the int values of -16 and 15
 * Notes:
 *      * The resulting float will be between -0.3 and 0.3
 ************************/
float unquantized_5bit(int64_t five_bit)
{
        float unquantized = five_bit * BCD_CODE;
        return unquantized;
}

