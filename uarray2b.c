/*
 *     uarray2b.c
 *     by Kabir Pamnani and Alex Shriver, 02/22/2023
 *     HW3: Locality
 *
 *     Summary: Implementation of 2D Unboxed Blocked Arrays 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <uarray.h>

#define T UArray2b_T
#define SIXTY_FOUR_KB 65536

/*********************************
 *******      NOTE      **********
 ********************************/
/*
* It is a checked run-time error to pass a NULL T
* to any function in this interface
*/

/*
 * This is the struct definition of the UArray2b_T instance
 * Elements:
 *      UArray2_T uarray2: A UArray2_T holding elements that are each blocks. 
 *              These blocks are represented by UArray_T's, whose elements are
 *              each the elements stored by the overall UArray2b
 *      int width: the number of columns in the UArray2b
 *      int height: the number of rows in the UArray2b
 *      int size: The size, in bytes, of each UArray2b element
 *      int blocksize: The number of elements in a side of a block of the 
 *              UArray2b, thus each block (UArray_T) contains 
 *              blocksize * blocksize elements.
 *              
 */
struct T {
        UArray2_T uarray2;
        int width;
        int height;
        int size;
        int blocksize;
};

/**********UArray2b_new********
 *
 * Allocates, initializes and returns a new blocked UArray2 with width x height 
 * cells, each of which are of size bytes and have a blocksize that represents
 * the square root of the # of cells in each block.
 * Inputs:
 *              int width: integer storing the number of columns contained in 
 *                         the UArray2b
 *              int height: integer storing the number of rows contained in the
 *                          UArray2b
 *              int size: integer storing the size (in bytes) of each element
 *                        in the new UArray2b
 *              int blocksize: integer storing the blocksize of each block
 *                             in the new UArray2b
 * Return: A new UArray2b with width x height number of elements, each of which
 *         are of size bytes, divided into blocks of size blocksize * blocksize 
 * Expects:
 *      * width and height to be nonnegative
 *      * size to be positive
 *      * blocksize to be positive
 * Notes:
 *      Checked runtime error if:
 *              * width or height is negative
 *              * size is nonpositive
 *              * blocksize is nonpositive
 *      UArray2b can raise Mem_Failed if UArray2b_new can't allocate the memory
 *      requested
 *      The client must free heap allocated memory using UArray2b_free
 ************************/
T UArray2b_new (int width, int height, int size, int blocksize)
{
        T uarray2b = malloc(sizeof(*uarray2b));
        assert(uarray2b != NULL);
        assert(width >= 0); 
        assert(height >= 0);
        assert(blocksize >= 1);

        uarray2b->width = width;
        uarray2b->height = height;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;

        int block_width = width / blocksize;
        int block_height = height / blocksize;

        if (width % blocksize > 0) {
                block_width++;
        }
        if (height % blocksize > 0) {
                block_height++;
        }

        /* Instantiates UArray2 holding the blocks */
        uarray2b->uarray2 = UArray2_new(block_width, block_height, 
                                                        sizeof(UArray_T));

        /* Instantiates each UArray representing blocks, holding elements */
        for (int c = 0; c < block_width; c++) {
                for (int r = 0; r < block_height; r++) {
                        UArray_T *ua = UArray2_at(uarray2b->uarray2, c, r);
                        *ua = UArray_new(blocksize * blocksize, size);
                }
        }

        return uarray2b;
}

/**********UArray2b_new_64K_block********
 *
 * Allocates, initializes and returns a new blocked UArray2 with width x height 
 * cells, each of which are of size bytes and have a blocksize as large as 
 * possible provided a block occupies at most 64KB. Blocksize is defaulted to
 * 1 if a single cell will not fit in 64KB.
 * Inputs:
 *              int width: integer storing the number of columns contained in 
 *                         the UArray2b
 *              int height: integer storing the number of rows contained in the
 *                          UArray2b
 *              int size: integer storing the size (in bytes) of each element
 *                        in the new UArray2b
 * Return: A new UArray2b with width x height number of elements, each of which
 *         are of size bytes, and have a blocksize as large as possible 
 *         provided a blcok occupies at most 64KB, or a blocksize of 1 if a 
 *         cell is > 64KB.
 * Expects:
 *      * width and height to be nonnegative
 *      * size to be positive
 *      * blocksize to be positive
 * Notes:
 *      This function utilizes the UArray2b_new function, and thus it
 *      initializes the following checked runtime errors through 
 *      that function:
 *              * width or height is negative
 *              * size is nonpositive
 *              * blocksize is nonpositive
 *      UArray2b_new_64K_block can raise Mem_Failed if UArray2b_new can't 
 *      allocate the memory requested
 *      The client must free heap allocated memory using UArray2b_free
 ************************/
T UArray2b_new_64K_block(int width, int height, int size) 
{
        int blocksize = 1;
        if (!(size > SIXTY_FOUR_KB)) {
                blocksize = sqrt(SIXTY_FOUR_KB / size);
        }
        return UArray2b_new(width, height, size, blocksize);
}

/**********UArray2b_free********
 *
 * Deallocates and clears the *UArray2b
 * Inputs:
 *              T *array2b: Pointer to a pointer to the UArray2b that will be
 *                          deallocated and cleared
 * Return: N/A
 * Expects:
 *      UArray2b or *UArray2b to be nonnull
 * Notes:
 *      Checked runtime error if UArray2b / *UArray2b is null
 ************************/
void UArray2b_free(T *array2b) 
{
        for (int b_col = 0; 
                        b_col < UArray2_width((*array2b)->uarray2); b_col++) {
                for (int b_row = 0; 
                        b_row < UArray2_height((*array2b)->uarray2); b_row++) 
                {
                        UArray_T *ua = UArray2_at((*array2b)->uarray2, 
                                                                b_col, b_row);
                        UArray_free(&(*ua));
                }
        }
        UArray2_free(&((*array2b)->uarray2));
        free(*array2b);
}

/**********UArray2b_width********
 *
 * Returns the number of columns in the UArray2b
 * Inputs:
 *              T array2b: A pointer to the UArray2b in which the width is to 
 *                         be retrieved
 * Return: the number of columns in the UArray2b (ie the width of the array2b)
 * Expects:
 *       Pointer to the UArray2b to be nonnull
 * Notes:
 *      Checked runtime error if the UArray2b is null
 ************************/
int UArray2b_width (T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/**********UArray2b_height********
 *
 * Returns the number of rows in the UArray2b
 * Inputs:
 *              T array2b: A pointer to the UArray2b in which the height is to
 *                         be retrieved
 * Return: the number of rows in the UArray2b (ie the height of the UArray2b)
 * Expects:
 *       Pointer to UArray2b to be nonnull
 * Notes:
 *      Checked runtime error if the UArray2b is null
 ************************/
int UArray2b_height (T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/**********UArray2b_size********
 *
 * Returns the size of each element in the UArray2b
 * Inputs:
 *              T array2b: A pointer to the UArray2b in which the size of each
 *                         element will be retrieved
 * Return: the size (in bytes) of each of the elements in the UArray2b
 * Expects:
 *      Pointer to UArray2b to be nonnull
 * Notes:
 *      Checked runtime error if the UArray2b is null
 ************************/
int UArray2b_size (T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}

/**********UArray2b_blocksize********
 *
 * Returns the blocksize of the UArray2b
 * Inputs:
 *              T array2b: A pointer to the UArray2b in which the blocksize of 
 *                         each element will be retrieved
 * Return: the blocksize of the elements in the UArray2b (ie the square root
 *         of the number of cells in a block)
 * Expects:
 *      Pointer to UArray2b to be nonnull
 * Notes:
 *      Checked runtime error if the UArray2b is null
 ************************/
int UArray2b_blocksize (T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/**********UArray2b_at********
 *
 * Returns a pointer to the element within the UArray2b provided in the first
 * parameter, at the indices (col, row), where col is the distance between the 
 * element and the left edge of the UArray2b, and row is the distance between
 * the element and the top row of the UArray2b
 * Inputs:
 *              T array2b: A pointer to the UArray2b in which the element is to 
 *                         be located
 *              int col: The column index of the element within the UArray2b
 *              int row: The row index of the element within the UArray2b
 * Return: A pointer to the element within the UArray2b that is in the 
 *         indices (col, row)
 * Expects:
 *      * An existing UArray2b is entered in as the first parameter
 *      * The row value is positive and is less than the height of the UArray2b
 *      * The col value is positive and is less than the width of the UArray2b
 * Notes:
 *      Returns a pointer!
 *      * Checked runtime error if:
 *              * UArray2b is null 
 *              * row value >= height or row value < 0
 *              * col value >= width or col value < 0
 ************************/
void *UArray2b_at(T array2b, int col, int row)
{
        assert(array2b != NULL);
        assert(row >= 0 && row < array2b->height);
        assert(col >= 0 && col < array2b->width);
        int block_col = col / array2b->blocksize;
        int block_row = row / array2b->blocksize;

        UArray_T *ua = UArray2_at(array2b->uarray2, block_col, block_row);

        int ua_index = array2b->blocksize * (col % array2b->blocksize) 
                                                + (row % array2b->blocksize);
        return UArray_at(*ua, ua_index);
}

/**********UArray2b_map********
 *
 * Calls an apply function for each element in UArray2b, by visiting each cell
 * in one block before moving to another block
 * Inputs:
 *              T array2b: A pointer to the UArray2b that the apply function 
 *                         will be called on 
 *                         
 *              void apply: The function that will be applied to each element 
 *                          in UArray2b      * parameters detailed below *
 *                  int col: the current column index
 *                  int row: the current row index
 *                  T array2b: A pointer to the same UArray2b passed into the
 *                             outside function
 *                  void *elem: A pointer to the current element 
 *                                    at pos (col, row) in the UArray2b
 *                  void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 *
 *              void *cl: A closure passed in by the client to be used in the
 *                        apply function               
 *          
 * Return: N/A
 * Expects: 
 *      * UArray2b to be nonnull
 *      * The row value is positive and is less than the height of the UArray2b 
 *      * The col value is positive and is less than the width of the UArray2b
 * Notes:
 *      Updates the UArray2b entered in as the first parameter 
 ************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b, 
                                        void *elem, void *cl), void *cl) 
{       
        assert(array2b != NULL);
        int blocksize = array2b->blocksize;
        int width = array2b->width;
        int height = array2b->height;
        for (int b_col = 0; b_col < UArray2_width(array2b->uarray2); b_col++) {
                for (int b_row = 0; b_row < UArray2_height(array2b->uarray2); 
                                                                b_row++) 
                {
                        UArray_T *ua = UArray2_at(array2b->uarray2, b_col, 
                                                                        b_row);
                        for (int c = b_col * blocksize; 
                                c < b_col * blocksize + blocksize; c++) {
                                for (int r = b_row * blocksize; 
                                        r < b_row * blocksize + blocksize; r++) 
                                {
                                        if (c < width && r < height) 
                                        {
                                                int ua_index = blocksize * 
                                                        (c % blocksize) + 
                                                        (r % blocksize);
                                                apply(c, r, array2b, UArray_at
                                                        (*ua, ua_index), cl);
                                        }
                                }        
                        }
                }
        }
}
