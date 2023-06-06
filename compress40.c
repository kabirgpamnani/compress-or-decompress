/********************************************************************
 *
 *                          compress40.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *     * Implementaion for compress40.h
 *
 *     Summary:
 *      
 *     compress40 either compresses a ppm image and writes result to standard 
 *     output, or decompresses a compressed image to a ppm image and writes the 
 *     result to standard output   
 * 
 *     Notes:
 *   - This file calls functions from modules rgbcomponent.h, compress2x2.h,
 *     readwritecompressed.h, a2methods.h, a2blocked.h, and uarray2b,
 *     to compress and decompress the images as appropriate
 *     
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "rgbcomponent.h"
#include "compress2x2.h"
#include "readwritecompressed.h"

// TODO:
// 1) FINISH LAST FUNCTION CONTRACTS - DONE
// 2) WE NEED TO CHANGE THE SECOND FUNCTION IN READWRITECOMPRESSED TO CRE IF THE
// SUPPLIED FILE IS TOO SHORT (TOP OF PAGE 6 IN THE SPEC)
// 2) STRUCT CONTRACTS
// 3) HEADERS FOR EACH FILE (LOOK AT COMMENTS FROM LOCALITY), CHECK IF WE NEED
// FOR .H FILES
// 4) INLINE COMMENTS
// 5) ASK ABOUT SECRET STUFF + IF OUR MODULES ARE SPLIT LOGICALLY??
// 6) CHECK WE HAVE ASSERTED EVERYWHERE
// 7) CHECK VALGRIND AGAIN!
//8) TEST MORE + RUN THROUGH LOGIC
// 9) Read thru spec to make sure we didnt miss anything

/**********compress40********
 *
 * Reads in a PPM file, and writes a compressed binary image file to standard
 * output 
 * Inputs:
 *              FILE *input: pointer to the input PPM file
 * Return: N/A
 * Expects:
 *      * pointer to input PPM file to be nonnull
 * Notes:
 *      * Checked runtime error if:
 *              * pointer to input PPM file is NULL
 ************************/
void compress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods != NULL);

        Pnm_ppm og_image = Pnm_ppmread(input, methods);
        Pnm_ppm image_trimmed = trimmed_image(og_image);
        UArray2b_T componentUArray2b = RGBtoComponentVideo(image_trimmed);
        UArray2b_T compressed_blocks = compressed2x2s(componentUArray2b);
        print_to_stdout(compressed_blocks);
} 

/**********decompress40********
 *
 * Reads in a compressed binary image, and writes an decompressed PPM image to
 * standard output
 * Inputs:
 *              FILE *input: pointer to the inputted compressed binary 
 *                           image file
 * Return: N/A
 * Expects:
 *      * pointer to input file to be nonnull
 * Notes:
 *      * Checked runtime error if:
 *              * pointer to input file is NULL
 ************************/
void decompress40(FILE *input)
{
        assert(input != NULL);
        UArray2b_T compressed_blocks = read_compressed_file(input);
        UArray2b_T decompressed_component = decompressed2x2s(compressed_blocks);
        Pnm_ppm decompressed_to_rgb = 
                                ComponentVideotoRGB(decompressed_component);
        Pnm_ppmwrite(stdout, decompressed_to_rgb);
        Pnm_ppmfree(&decompressed_to_rgb);
}  