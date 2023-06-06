/********************************************************************
 *
 *                          bitpack.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Implementation for bitpack.h
 *
 *     Summary:
 *      bitpack includes functions that allow for the manipulation of bit fields
 *      with a 64 bit (8 byte) value word. This includes functions that will 
 *      pack, retrieve and update bitfields.
 * 
 *     Notes:
 *   - This module does not use functions from other modules
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "bitpack.h"
#include "except.h"
Except_T Bitpack_Overflow = { "Overflow packing bits" };

#define WORD_SIZE 64 

static uint64_t left_shift(uint64_t value, unsigned shift_value);
static uint64_t right_shift(uint64_t value, unsigned shift_value);
static int64_t right_shift_signed(int64_t value, unsigned shift_value);

/**********left_shift********
 *
 * Left shifts an unsigned 64 bit int value by the shift_value amount
 * Inputs:
 *              uint64_t value: The 64 bit unsigned int being shifted
 *              unsigned shift_value: The amount of the left shift
 * Return: value after being left shifted
 * Expects:
 *      the shift_value to be between 0 and the WORD_SIZE, which is 64 in this 
 *      case
 * Notes:
 *      * if the shift_value is 64, then replace value with all 0s in its bits
 ************************/
static uint64_t left_shift(uint64_t value, unsigned shift_value)
{
        assert(shift_value <= WORD_SIZE);
        if (shift_value <= WORD_SIZE - 1) {
                value = (value << shift_value);
        } else {
                value = (uint64_t)0;
        }
        return value;
}

/**********right_shift********
 *
 * Right shifts an unsigned 64 bit int value by the shift_value amount
 * Inputs:
 *              uint64_t value: The 64 bit unsigned int being shifted
 *              unsigned shift_value: The amount of the right shift
 * Return: value after being right shifted
 * Expects:
 *      the shift_value to be between 0 and the WORD_SIZE, which is 64 in this 
 *      case
 * Notes:
 *      * if the shift_value is 64, then replace value with all 0s in its bits
 ************************/
static uint64_t right_shift(uint64_t value, unsigned shift_value)
{
        assert(shift_value <= WORD_SIZE);
        if (shift_value <= WORD_SIZE - 1) {
                value = (value >> shift_value);
        } else {
                value = (uint64_t)0;
        }
        return value;
}

/**********right_shift_signed********
 *
 * Right shifts a signed 64 bit int value by the shift_value amount
 * Inputs:
 *              int64_t value: The 64 bit signed int being shifted
 *              unsigned shift_value: The amount of the right shift
 * Return: value after being right shifted
 * Expects:
 *      the shift_value to be between 0 and the WORD_SIZE, which is 64 in this 
 *      case
 * Notes:
 *      * if the shift_value is 64 and value is less than 0, 
 *              then replace value with all 1s in its bits
 *      * if the shift_value is 64 and value is greater than or equal to 0, 
 *              then replace value with all 0s in its bits
 ************************/
static int64_t right_shift_signed(int64_t value, unsigned shift_value)
{
        assert(shift_value <= WORD_SIZE);
        if (shift_value <= WORD_SIZE - 1) {
                value = (value >> shift_value);
        } else if (value < 0){
                value = ~(int64_t)0;
        } else {
                value = (int64_t)0;
        }

        return value;
}

/**********Bitpack_fitsu********
 *
 * Tells whether argument n (unsigned) can fit in width bits 
 * Inputs:
 *              uint64_t n: The 64 bit unsigned int being checked
 *              unsigned width: The amount of bits being checked with n
 * Return: True if n (unsigned) can fit in width bits, false if it can't
 * Expects:
 *      width to be between 0 and the WORD_SIZE, which is 64 in this case
 * Notes:
*       N/A
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= WORD_SIZE);
        uint64_t max_num = (uint64_t)left_shift((uint64_t)1, width);

        if (max_num == 0) {
                max_num = ~(uint64_t)0;
        } else {
                max_num -= 1;
        }

        if (n <= max_num) {
                return true;
        } else {
                return false;
        }
}

/**********Bitpack_fitss********
 *
 * Tells whether argument n (signed) can fit in width bits 
 * Inputs:
 *              int64_t n: The 64 bit signed int being checked
 *              unsigned width: The amount of bits being checked with n
 * Return: True if n (signed) can fit in width bits, false if it can't
 * Expects:
 *      width to be between 0 and the WORD_SIZE, which is 64 in this case
 * Notes:
*       N/A
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= WORD_SIZE);

        int64_t max_num = (int64_t)0;
        int64_t min_num = (int64_t)0;

        if (width != 0) {
                max_num = (int64_t)left_shift((uint64_t)1, width - 1) - 1;
                min_num = ~max_num;
        }

        // printf("signed maxval: %ld\n", max_num);
        // printf("signed minval: %ld\n", min_num);

        //possibly max_num / 2 could be too big to be an int64_t
        //might need a <= typa vibe
        if (n >= min_num && n <= max_num) {
                return true;
        } else {
                return false;
        }
}

/**********Bitpack_getu********
 *
 * Extracts and returns a field from a word given the width of the field and
 * the location of the field's least significant bit
 * Inputs:
 *              uint64_t word: the 64 bit unsigned word that the value will be
 *                             extracted from 
 *              unsigned width: the width of the field to be updated
 *              unsigned lsb: the location of the least significant bit of the 
 *                            field to be extracted
 * Return: the 64 bit unsigned field from the given word 
 * Expects:
 *      width to be <= size of word (64 bits)
 *      width + lsb to be <= size of word (64 bits) 
 * Notes:
 *      * checked runtime error if:
 *              * width > size of word (64 bits)
 *              * width + lsb > size of word (64 bits)
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= WORD_SIZE);
        assert(width + lsb <= WORD_SIZE);
        uint64_t mask = ~(uint64_t)0;
        mask = right_shift(mask, (WORD_SIZE - width));
        mask = left_shift(mask, lsb);
        uint64_t extracted_word = word & mask;
        extracted_word = right_shift(extracted_word, lsb);

        return extracted_word;
}

/**********Bitpack_gets********
 *
 * Extracts and returns a field from a word given the width of the field and
 * the location of the field's least significant bit
 * Inputs:
 *              uint64_t word: the 64 bit unsigned word that the value will be
 *                             extracted from 
 *              unsigned width: the width of the field to be updated
 *              unsigned lsb: the location of the least significant bit of the 
 *                            field to be extracted
 * Return: the 64 bit signed field from the given word 
 * Expects:
 *      width to be <= size of word (64 bits)
 *      width + lsb to be <= size of word (64 bits) 
 * Notes:
 *      * checked runtime error if:
 *              * width > size of word (64 bits)
 *              * width + lsb > size of word (64 bits)
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= WORD_SIZE);
        assert(width + lsb <= WORD_SIZE);
        uint64_t mask = ~(uint64_t)0;
        mask = right_shift(mask, (WORD_SIZE- width));
        mask = left_shift(mask, lsb);
        uint64_t extracted_word = (word & mask);
        extracted_word = left_shift(extracted_word, WORD_SIZE - (width + lsb));
        int64_t signed_extracted_word = right_shift_signed(extracted_word, 
                                                                64 - width);

        return signed_extracted_word;
}

/**********Bitpack_newu********
 *
 * Returns a new word that is identical to the original word, except that the
 * field of width width with least significant bit at lsb will have been 
 * replaced by a width-bit representation of value. Supports only unsigned 
 * values.
 * Inputs:
 *              uint64_t word: the original word that the value of the field 
 *                             will be extracted and updated from 
 *              unsigned width: the width of the field to be updated
 *              unsigned lsb: the location of the least significant bit of the 
 *                            field to be updated 
 *              uint64_t value: the value that will replace the original 
 *                             field value (value is unsigned)              
 * Return: a new 64 bit unsigned word with the updated field value
 * Expects:
 *      width to be <= size of word (64 bits)
 *      width + lsb to be <= size of word (64 bits) 
 *      value to fit in the given width of the field
 * Notes:
 *      * checked runtime error if:
 *              * width > size of word (64 bits)
 *              * width + lsb > size of word (64 bits)
 *      * raises exception "Bitpack_Overflow" if value does not fit in width of
 *        field
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                        uint64_t value)
{
        assert(width <= WORD_SIZE);
        assert(width + lsb <= WORD_SIZE);

        if ((Bitpack_fitsu(value, width)) == false) {
                RAISE(Bitpack_Overflow);
        }

        value = left_shift(value, lsb);
        uint64_t mask_h = ~(uint64_t)0;
        mask_h = left_shift(mask_h, lsb + width);
        uint64_t mask_l = ~(uint64_t)0;
        mask_l = right_shift(mask_l, WORD_SIZE - lsb);
        uint64_t mask = mask_h | mask_l;
        uint64_t updated_word = word & mask;
        updated_word = updated_word | value;
        return updated_word;
}

/**********Bitpack_news********
 *
 * Returns a new word that is identical to the original word, except that the
 * field of width width with least significant bit at lsb will have been 
 * replaced by a width-bit representation of value. Supports only signed 
 * values.
 * Inputs:
 *              uint64_t word: the 64 bit signed original word that the value 
 *                             of the field will be extracted and updated from 
 *              unsigned width: the width of the field to be updated
 *              unsigned lsb: the location of the least significant bit of the 
 *                            field to be updated 
 *              int64_t value: the value that will replace the original 
 *                             field value (value is signed)              
 * Return: a new 64 bit unsigned word with the updated field value
 * Expects:
 *      width to be <= size of word (64 bits)
 *      width + lsb to be <= size of word (64 bits) 
 *      value to fit in the given width of the field
 * Notes:
 *      * checked runtime error if:
 *              * width > size of word (64 bits)
 *              * width + lsb > size of word (64 bits)
 *      * raises exception "Bitpack_Overflow" if value does not fit in width of
 *        field
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                                int64_t value)
{
        assert(width <= WORD_SIZE);
        assert(width + lsb <= WORD_SIZE);

        if ((Bitpack_fitss(value, width)) == false) {
                RAISE(Bitpack_Overflow);
        }

        value = left_shift(value, WORD_SIZE - width);
        value = right_shift(value, WORD_SIZE - width);

        value = left_shift(value, lsb);
        uint64_t mask_h = ~(uint64_t)0;
        mask_h = left_shift(mask_h, lsb + width);
        uint64_t mask_l = ~(uint64_t)0;
        mask_l = right_shift(mask_l, WORD_SIZE - lsb);
        uint64_t mask = mask_h | mask_l;
        uint64_t updated_word = word & mask;
        updated_word = updated_word | value;
        return updated_word;
}

