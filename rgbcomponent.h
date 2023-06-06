/********************************************************************
 *
 *                          rgbcomponent.h
 *
 *     Assignment: arith
 *     Authors:    Kabir Pamnani & Isaac Monheit 
 *     Date:       March 9th, 2023 
 *
 *      * Interface for rgbcomponent.c
 *
 *     Summary:
 *      rgbcomponent either transforms a ppm image from RGB color space to 
 *      component video color space (CVS) and returns a 2d blocked array that 
 *      holds the resulting values, or transforms a 2d blocked array holding CVS
 *      values to a ppm image that holds RGB color space values.
 * 
 *******************************************************************/
#ifndef RGBCOMPONENT_INCLUDED
#define RGBCOMPONENT_INCLUDED

typedef struct CVS *CVS;

/*
 * This is the struct definition of the CVS instance
 * Elements:
 *      float y: a float value y which represents the luminance of each pixel
 *      float pb: pb value (transmits color-difference signals) in float form
 *      float pr: pb value (transmits color-difference signals) in float form
 *              
 */
struct CVS {
        float y;
        float pb;
        float pr;
};


/* conversion to Component Video color space */
UArray2b_T RGBtoComponentVideo(Pnm_ppm trimmed_image);
void onePixelToComponentVideo(int col, int row, UArray2b_T componentVideo, 
                                        void *elem, void *trimmed_image);

/* conversion to RGB color space */
Pnm_ppm ComponentVideotoRGB(UArray2b_T componentVideo);
void onePixelToRGB(int col, int row, UArray2b_T componentVideo, void *elem, 
                                                             void *rgb_pixmap);
Pnm_rgb CVS_to_RGB(CVS one_pixel, Pnm_rgb rgb_struct);

/* 
 * trimming the image (neccesary for coversion to Component Video color 
 * space) 
 */
Pnm_ppm trimmed_image(Pnm_ppm original_image);
void og_to_new(int col, int row, A2Methods_UArray2 A2uarray2, 
                                         void *elem, void *original_image);
#endif