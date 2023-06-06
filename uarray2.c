/*
 *     uarray2.c
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Implementation of 2D Unboxed Arrays 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uarray2.h"
#include "uarray.h"
#include "uarrayrep.h"

#define T UArray2_T 


struct T {
        UArray_T uarray;
        int width;
        int height;
        int size;
};

/**********UArray2_new********
 *
 * Allocates, initializes and returns a new UArray2 with width x height 
 * elements, each of which are of size bytes
 * Inputs:
 *              int width: integer storing the number of columns contained in 
 *                         the UArray2
 *              int height: integer storing the number of rows contained in the
 *                          UArray2
 *              int size: integer storing the size (in bytes) of each element
 *                          in the new UArray2
 * Return: A new UArray2 with width x height number of elements, each of which
 *         are of size bytes
 * Expects:
 *      * width and height to be nonnegative
 *      * size to be positive
 * Notes:
 *      Checked runtime error if width or height is negative, or if size
 *      is nonpositive
 *      UArray2 can raise Mem_Failed if UArray2_new cannot allocate the memory
 *      requested
 ************************/
T UArray2_new(int width, int height, int size)
{
        T uarray2 = malloc(sizeof(*uarray2));
        assert(uarray2 != NULL);
    
        assert(width >= 0); 
        assert(height >= 0);
        assert(size > 0);
        uarray2->width = width;
        uarray2->height = height;
        uarray2->size = size;

        uarray2->uarray = UArray_new(width * height, size);

        return uarray2;
}

/**********UArray2_width********
 *
 * Returns the number of columns in the UArray2
 * Inputs:
 *              T uarray2: A pointer to the UArray2 in which the width is to 
 *                         be retrieved
 * Return: the number of columns in the UArray2 (ie the width of the uarray2)
 * Expects:
 *       Pointer to UArray2 to be nonnull
 * Notes:
 *      Checked runtime error if UArray2 is null
 ************************/
int UArray2_width(T uarray2) 
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL); 
        return uarray2->width;
}

/**********UArray2_height********
 *
 * Returns the number of rows in the UArray2
 * Inputs:
 *              T uarray2: A pointer to the UArray2 in which the height is to
 *                         be retrieved
 * Return: the number of rows in the UArray2 (ie the height of the UArray2)
 * Expects:
 *       Pointer to UArray2 to be nonnull
 * Notes:
 *      Checked runtime error if UArray2 is null
 ************************/
int UArray2_height(T uarray2)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL); 
        return uarray2->height;
}

/**********UArray2_size********
 *
 * Returns the size of each element in the UArray2
 * Inputs:
 *              T uarray2: A pointer to the UArray2 in which the size of each
 *                         element will be retrieved
 * Return: the size (in bytes) of each of the elements in the UArray2
 * Expects:
 *      Pointer to UArray2 to be nonnull
 * Notes:
 *      Checked runtime error if UArray2 is null
 ************************/
int UArray2_size (T uarray2) 
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL); 
        return uarray2->size;
}

/**********UArray2_free********
 *
 * Deallocates and clears the *UArray2
 * Inputs:
 *              T *uarray2: Pointer to a pointer to the UArray2 that will be
 *                          deallocated and cleared
 * Return: N/A
 * Expects:
 *      uarray2 or *uarray2 to be nonnull
 * Notes:
 *      Checked runtime error if uarray / *uarray2 to be null
 ************************/
void UArray2_free(T *uarray2)
{
        UArray_free(&((*uarray2)->uarray));
        free(*uarray2);
}

/**********UArray2_at********
 *
 * Returns a pointer to the element within the UArray2 provied in the first
 * parameter, at the indices (row, col), where row is is the distance between
 * the element and the top row of the UArray2, and col is the between the 
 * element and the left edge of the UArray2
 * Inputs:
 *              T uarray2: A pointer to the UArray2 in which the element is to 
 *                         be located
 *              int row: The row index of the element within the UArray2
 *              int col: The column index of the element within the UArray2
 * Return: A pointer to the element within the UArray2 that is in the 
 *         indices (row, col)
 * Expects:
 *      * An existing UArray2 is entered in as the first parameter
 *      * The row value is positive and is less than the height of the UArray2
 *      * The col value is positive and is less than the width of the UArray2
 * Notes:
 *      Returns a pointer!
 *      * Checked runtime error if:
 *              * UArray2 is null 
 *              * row value >= height or row value < 0
 *              * col value >= width or col value < 0
 ************************/
void *UArray2_at(T uarray2, int col, int row)
{
        assert(uarray2 != NULL);
        assert(row >= 0 && row < uarray2->height);
        assert(col >= 0 && col < uarray2->width);
        return UArray_at(uarray2->uarray, row * uarray2->width + col);
}

/**********UArray2_map_row_major********
 *
 * Calls an apply function for each element in UArray2, in order from low to 
 * high indices, with column indices varying more rapidly than row indices
 * Inputs:
 *              T uarray2: A pointer to the UArray2 that the apply function 
 *                         will be called on 
 *                         
 *              void apply: The function that will be applied to each element 
 *                          in UArray2      * parameters detailed below *
 *                  int row: the current row index
 *                  int col: the current column index
 *                  T uarray2: A pointer to the same UArray2 passed into the
 *                             outside function
 *                  void *element_at: A pointer to the current element 
 *                                    at pos (row, col) in the UArray2
 *                  void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 *
 *              void *cl: A closure passed in by the client to be used in the
 *                        apply function               
 *                     
 *          
 * Return: N/A
 * Expects: 
 *      * UArray2 to be nonnull
 *      * The row value is positive and is less than the height of the UArray2 
 *      * The col value is positive and is less than the width of the UArray2
 * Notes:
 *      Updates the UArray2 entered in as the first parameter 
 ************************/
void UArray2_map_row_major(T uarray2, void apply(int col, int row, T uarray2, 
                                        void *element_at, void *cl), void *cl)
{
        assert(uarray2 != NULL);
        for (int r = 0; r < uarray2->height; r++) {
                for (int c = 0; c < uarray2->width; c++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}

/**********UArray2_map_col_major********
 *
 * Calls an apply function for each element in UArray2, in order from low to 
 * high indices, with row indices varying more rapidly than col indices
 * Inputs:
 *              T uarray2: A pointer to the UArray2 that the apply function 
 *                         will be called on 
 *                         
 *              void apply: The function that will be applied to each element 
 *                          in UArray2      * parameters detailed below *
 *                  int row: the current row index
 *                  int col: the current column index
 *                  T uarray2: A pointer to the same UArray2 passed into the
 *                             outside function
 *                  void *element_at: A pointer to the current element 
 *                                    at pos (row, col) in the UArray2
 *                  void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 *
 *              void *cl: A closure passed in by the client to be used in the
 *                        apply function               
 *          
 * Return: N/A
 * Expects: 
 *      * UArray2 to be nonnull
 *      * The row value is positive and is less than the height of the UArray2 
 *      * The col value is positive and is less than the width of the UArray2
 * Notes:
 *      Updates the UArray2 entered in as the first parameter 
 ************************/
void UArray2_map_col_major(T uarray2, void apply(int col, int row, T uarray2, 
                                        void *element_at, void *cl), void *cl)
{
        assert(uarray2 != NULL);
        for (int c = 0; c < uarray2->width; c++) {
                for (int r = 0; r < uarray2->height; r++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}



