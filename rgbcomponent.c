/********************************************************************
 *
 *                          rgbcomponent.c
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Implementation for rgbcomponent.h
 *
 *     Summary:
 *      rgbcomponent either transforms a ppm image from RGB color space to 
 *      component video color space (CVS) and returns a 2d blocked array that 
 *      holds the resulting values, or transforms a 2d blocked array holding CVS
 *      values to a ppm image that holds RGB color space values.
 * 
 *     Notes:
 *   - This module also contains functions that trim the last row and/or column
 *     as necessary
 *   - This module calls function from these other modules: a2methods.h, 
 *     a2blocked.h and uarray2b.h
 *******************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "rgbcomponent.h"

#define DENOMINATOR 255
#define NEWBLOCKSIZE 2

/**********trimmed_image********
 *
 * Trims off the last row and/or column of an inputted ppm image if necessary 
 * to make the image have an even width and height
 * Inputs:
 *              Pnm_ppm original_image: the ppm image that is to be trimmed if
 *                                      needed
 * Return: a ppm image in Pnm_ppm format that has an even width and 
 *         height value
 * Expects:
 *      * original_image to be non NULL
 * Notes:
 *      * frees up memory for the inputted pnm_ppm image, and allocates memory
 *        for the returned image. The caller assumes ownership of the returned
 *        image
 *      * checked runtime error if:
 *              * original_image is NULL
 ************************/
Pnm_ppm trimmed_image(Pnm_ppm original_image) 
{
        assert(original_image != NULL);

        int new_height = original_image->height;
        int new_width = original_image->width;

        if (original_image->height % 2 == 1) {
                new_height = new_height - 1;
        } 
        if (original_image->width % 2 == 1) {
                new_width = new_width - 1;
        }

        /* trimmed image pixels array for new Ppm_pnm */
        A2Methods_UArray2 new_image_array = original_image->methods->
                        new_with_blocksize(new_width, new_height, 
                                original_image->methods->
                                        size(original_image->pixels), 
                                                                NEWBLOCKSIZE);

        /* populate new array with the pixels from the old image */
        original_image->methods->map_block_major(new_image_array, og_to_new, 
                                                                original_image);
        
        Pnm_ppm new_image = malloc(sizeof(struct Pnm_ppm));
        new_image->width = new_width;
        new_image->height = new_height;
        new_image->denominator = original_image->denominator;
        new_image->pixels = new_image_array;
        new_image->methods = original_image->methods;

        Pnm_ppmfree(&original_image);

        return new_image;
}

/**********og_to_new********
 *
 * Copies one pixel from one UArray2b to another new UArray2b
 * Inputs:
 *              int col: the column value of the current position in the 
 *                       A2Methods_UArray2 A2uarray2
 *              int row: the row value of the current position in the 
 *                       A2Methods_UArray2 A2uarray2
 *              A2Methods_UArray2 A2uarray2: An A2Methods_UArray2 
 *                        representing the image to be copied to 
 *                              (voided in this function)
 *              void *elem: A pointer to the element in the A2Methods_UArray2 
 *                          at position (col, row)
 *              void *original_image: An A2Methods_UArray2 representing the 
 *                                    image to be copied from
 * Return: N/A
 * Expects:
 *      None
 * Notes:
 *      * to be used in a methods->map function
 *      * in the map, copies every pixel in the bounds of the
 *        new A2Methods_UArray2 A2uarray2's width and height
 ************************/
void og_to_new(int col, int row, A2Methods_UArray2 A2uarray2, void *elem, 
                                                        void *original_image) 
{
        (void)A2uarray2;
        /* 
         * places the pixel at (col, row) in the old image into (col, row) of
         * the new image
         */
        *(Pnm_rgb)elem = (*(Pnm_rgb)(((Pnm_ppm)original_image)->methods)
                        ->at(((Pnm_ppm)original_image)->pixels, col, row));
}

/**********RGBtoComponentVideo********
 *
 * Transforms each pixel of a ppm image from RGB color space into 
 * component video color space and stores the resulting pixels in a UArray2b 
 * Inputs:
 *              Pnm_ppm trimmed_image: the trimmed ppm image that is to be 
 *                                     converted into component video color 
 *                                     space
 * Return: a UArray2b that stores the resulting pixels after they have been 
 *         converted to component video color space
 * Expects:
 *      * trimmed_image to be nonnull
 * Notes:
 *      * frees up memory for the inputted pnm_ppm image, and allocates memory
 *        for the UArray2b that will be returned. The caller assumes ownership 
 *        of the returned UArray2b.
 *      * checked runtime error if:
 *              * trimmed_image is NULL
 ************************/
UArray2b_T RGBtoComponentVideo(Pnm_ppm trimmed_image)
{
        assert(trimmed_image != NULL);
        UArray2b_T componentVideo = UArray2b_new(trimmed_image->width, 
                                trimmed_image->height, sizeof(struct CVS), 2);

        UArray2b_map(componentVideo, onePixelToComponentVideo, &trimmed_image);
        Pnm_ppmfree(&trimmed_image);

        return componentVideo;
}

/**********onePixelToComponentVideo********
 *
 * Converts one pixel in the ppm image from RGB color space to component video
 * color space
 * Inputs:
 *              int col: the column value of the current position of the pixel 
 *                       in the ppm image
 *              int row: the row value of the current position of the pixel
 *                       in the ppm image
 *              UArray2b_T componentVideo (voided): a UArray2b storing the 
 *                                                  resulting converted pixels
 *              void *elem: a pointer to the element in the ppm image 
 *                          at position (col, row)
 *              void *trimmed_image: ppm image that stores the pixels to be 
 *                                   converted
 * Return: N/A
 * Expects:
 *      trimmed_image to be nonnull
 *      one_pixel to be nonnull (created within function, not passed in)
 * Notes:
 *      * to be used as an apply function in the UArray2b_map function
 *      * in the map, copies each pixel (after it is converted to component 
 *        video space) into a new UArray2b
 *      * checked runtime error if
 *              * trimmed_image is NULL
 *              * one_pixel is NULL
 ************************/
void onePixelToComponentVideo(int col, int row, UArray2b_T componentVideo, 
                                                void *elem, void *trimmed_image)
{
        (void)componentVideo;

        assert(trimmed_image != NULL);
        CVS one_pixel = malloc(sizeof(struct CVS));
        assert(one_pixel != NULL);

        Pnm_ppm image = *(Pnm_ppm *)trimmed_image;
        Pnm_rgb rgb_struct = image->methods->at(image->pixels, col, row);

        float r = (float)rgb_struct->red / (float)image->denominator;
        float g = (float)rgb_struct->green / (float)image->denominator;
        float b = (float)rgb_struct->blue / (float)image->denominator;
        
        one_pixel->y = 0.299 * r + 0.587 * g + 0.114 * b;
        one_pixel->pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        one_pixel->pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

        /* set current element to its corresponding CVS struct */
        *(CVS)elem = *one_pixel;
        free(one_pixel);
}

/**********ComponentVideotoRGB********
 *
 * Transforms every pixel in a UArray2b from component video color space to 
 * RGB color space and stores the result in a ppm image 
 * Inputs:
 *              UArray2b_T componentVideo: A 2D blocked array of component
 *                                         video color space values (CVS) that
 *                                         represent a ppm image
 * Return: a ppm image in Pnm_ppm format that is filled with RGB values
 *         calculated from the CVS struct for each pixel
 * Expects:
 *      * componentVideo to be non NULL
 *      * A2Methods_T methods to be non NULL 
*               (note: not inputted to the function)
 * Notes:
 *      * frees up the memory used for the inputted UArray2b_T, and allocates 
 *        memory for the returned ppm image. The caller assumes ownership of 
 *        the returned image
 *      * checked runtime error if:
 *              * componentVideo is NULL
 *              * A2Methods_T methods is NULL
 ************************/
Pnm_ppm ComponentVideotoRGB(UArray2b_T componentVideo) 
{
        assert(componentVideo != NULL);

        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);
        Pnm_ppm rgb_image = malloc(sizeof(struct Pnm_ppm)); 
        
        int rgb_width = UArray2b_width(componentVideo);
        int rgb_height = UArray2b_height(componentVideo);

        /* creates a pixmap for the new Pmn_ppm */
        A2Methods_UArray2 rgb_pixmap =
                methods->new(rgb_width, rgb_height, sizeof(struct Pnm_rgb));
        
        rgb_image->width = rgb_width;
        rgb_image->height = rgb_height;
        rgb_image->denominator = DENOMINATOR;
        rgb_image->pixels = rgb_pixmap;
        rgb_image->methods = methods;

        /* populates the new pixmap with converted CVS structs */
        UArray2b_map(componentVideo, onePixelToRGB, rgb_image);

        UArray2b_free(&componentVideo);

        return rgb_image;
}

/**********onePixelToRGB********
 *
 * Converts one component video color space (CVS) pixel in the 
 * componentVideo UArray2b to RGB and adds to a pixmap for the result image
 * Inputs:
 *              int col: the column value of the current position of the pixel 
 *                       in the componentVideo UArray2b
 *              int row: the row value of the current position of the pixel
 *                       in the componentVideo UArray2b
 *              UArray2b_T componentVideo: An A2Methods_UArray2 
 *                        representing the image in CVS
 *              void *elem: A pointer to the element in the A2Methods_UArray2 
 *                          at position (col, row)
 *              void *rgb_pixmap: An A2Methods_UArray2 representing the 
 *                                RGB values of the result image
 * Return: N/A
 * Expects:
 *      * rgb_pixmap to be non NULL
 *      * rgb_struct to be non NULL (created within function, not passed in)
 * Notes:
 *      * to be used as an apply function in the UArray2b_map function
 *      * in the map, copies each pixel from CVS to RGB color space
 *      * checked runtime error if:
 *              * rgb_pixmap is NULL
 *              * rgb_struct is NULL
 ************************/
void onePixelToRGB(int col, int row, UArray2b_T componentVideo, void *elem, 
                                                             void *rgb_pixmap) 
{
        (void)elem;
        assert(rgb_pixmap != NULL);

        CVS one_pixel = (CVS)UArray2b_at(componentVideo, col, row);
        Pnm_rgb rgb_struct = malloc(sizeof(struct Pnm_rgb));
        assert(rgb_struct != NULL);
        
        /* 
         * places the converted RGB struct into the new pixmap at the current
         * column and row
         */
        (*(Pnm_rgb)(((Pnm_ppm)rgb_pixmap)->methods)->
                at(((Pnm_ppm)rgb_pixmap)->pixels, col, row)) = 
                                        *CVS_to_RGB(one_pixel, rgb_struct);
        free(rgb_struct);
}

/**********CVS_to_RGB********
 *
 * Converts the float values in a component video space struct into rgb integer 
 * values, and stores those values in an RGB struct.
 * Inputs:
 *              CVS one_pixel: a CVS struct that stores the float values to be
 *                             converted
 *              Pnm_rgb rgb_struct: an Pnm_rgb struct that stores the resulting
 *                                  int values after they have been converted
 * Return: returns an RGB struct that holds the rgb int values
 * Expects:
 *      * one_pixel to be nonnull
 *      * rgb_struct to be nonnull
 * Notes:
 *      * checked runtime error if:
 *              * one_pixel is NULL
 *              * rgb_struct is NULL
 ************************/
Pnm_rgb CVS_to_RGB(CVS one_pixel, Pnm_rgb rgb_struct)
{
        assert(one_pixel != NULL);
        assert(rgb_struct != NULL);

        float y = one_pixel->y;
        float pb = one_pixel->pb;
        float pr = one_pixel->pr;
        
        float r = 1.0 * y + 0.0 * pb + 1.402 * pr;
        float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        float b = 1.0 * y + 1.772 * pb + 0.0 * pr;

        r = fmax(0, fmin(1, r));
        g = fmax(0, fmin(1, g));
        b = fmax(0, fmin(1, b));
 
        rgb_struct->red = (int)(round(r * DENOMINATOR));
        rgb_struct->green = (int)(round(g * DENOMINATOR));
        rgb_struct->blue = (int)(round(b * DENOMINATOR));

        return rgb_struct;
}

