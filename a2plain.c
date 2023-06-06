/*
 *     a2plain.c
 *     by Kabir Pamnani and Alex Shriver, 02/25/2023
 *     HW3: locality
 *
 *     Summary: Implements a A2Methods_T methods suite using the UArray2 
 *              interface.
 */

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"


typedef A2Methods_UArray2 A2;   /* private abbreviation */

/*********************************
 *******      NOTE      **********
 ********************************/
/* 
 * All references to A2Methods_UArray2 instances or A2 instances are UArray2_t
 * objects in the methods suite created by this file.
 */



/**********new********
 *
 * Returns a newly instantiated A2Methods_UArray2, which is a UArray2_T, using
 * the UArray2_new function. Elements are size bytes.
 *
 * Inputs:
 *              int width: the width of the new UArray2_T
                int height: the height of the new UArray2_T 
                int size: the size of any given element in the UArray2
 * Return: An A2Methods_UArray2, which is a UArray2
 * Expects:
 *      * height and width must be nonnegative, and size must be positive.
 * Notes:
 *      * The client must free heap allocated memory using the a2free function 
 *              in this methods suite
 *      * This function solely utilizes the UArray2_new function, and thus it
 *        initializes the following checked runtime errors through 
 *        that function:
 *              * width >= 0
 *              * col >= 0
 *              * size > 0
 ************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/**********new_with_blocksize********
 *
 * Returns a newly instantiated A2Methods_UArray2, which is a UArray2_T, using
 * the UArray2_new function. Elements are size bytes.
 *
 * Inputs:
 *              int width: the width of the new UArray2_T
                int height: the height of the new UArray2_T 
                int size: the size of any given element in the UArray2
                int blocksize: an irrelevant argument designed for the blocked
                               implementation of an A2Methods_T
 * Return: An A2Methods_UArray2, which is a UArray2
 * Expects:
 *      * height and width must be nonnegative, and size must be positive.
 * Notes:
 *      * blocksize is voided
 *      * The client must free heap allocated memory using the a2free function 
 *              in this methods suite
 *      * This function solely utilizes the UArray2_new function, and thus it
 *        initializes the following checked runtime errors through 
 *        that function:
 *              * width >= 0
 *              * col >= 0
 *              * size > 0
 ************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/**********a2free********
 *
 * frees the memory allocated on the heap for an argued A2 element (which is a
 *      UArray2_T in this methods suite)
 *
 * Inputs:
 *              A2 *array2p: A pointer to an A2 instance to be freed.
 * Return: None (void function)
 * Expects:
 *      * both array2p and *array2p must be nonnull instances
 * Notes:
 *      * This function solely utilizes the UArray2_free function, and thus it
 *        initializes the following checked runtime errors through 
 *        that function:
 *              * array2p != NULL
 *              * *array2p != NULL
 ************************/
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/**********width********
 *
 * Returns the number of columns in the argued A2
 *
 * Inputs:
 *              A2 array2: The UArray2_T whose width is desired
 * 
 * Return: the number of columns in the A2 (ie the width of the array2)
 * Expects:
 *       array2 to be nonnull
 * Notes:
 *      Checked runtime error if array2 is null through UArray2_width function
 ************************/
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/**********height********
 *
 * Returns the number of rows in the argued A2
 *
 * Inputs:
 *              A2 array2: The UArray2_T whose height is desired
 * 
 * Return: the number of rows in the A2 (ie the height of the array2)
 * Expects:
 *       array2 to be nonnull
 * Notes:
 *      Checked runtime error if array2 is null through UArray2_width function
 ************************/
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/**********size********
 *
 * Returns the size of each element in the A2
 * Inputs:
 *              A2 array2: A pointer to the UArray2 in which the size of each
 *                         element will be retrieved
 * Return: the size (in bytes) of each of the elements in the A2
 * Expects:
 *      array2 to be nonnull
 * Notes:
 *      Checked runtime error if array2 is null through UArray2_size function
 ************************/
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/**********blocksize********
 *
 * This function is created for the blocked implementation of the A2Methods_T
 * methods suite. Because this file is not a blocked representation, each 
 * element in any applicable A2 will have blocks of only one element, because
 * the UArray2_T implementation organizes elements in a row
 *
 ************************/
static int blocksize(A2 array2)
{
        (void) array2;
        return 1;
}

/**********UArray2_at********
 *
 * Returns a pointer to the element within the A2 provied in the first
 * parameter, at the indices (col, row), where row is is the distance between
 * the element and the top row of the UArray2, and col is the between the 
 * element and the left edge of the UArray2
 * Inputs:
 *              A2 array2: A pointer to the UArray2 in which the element is to 
 *                         be located
 *              int row: The row index of the element within the UArray2
 *              int col: The column index of the element within the UArray2
 * Return: A pointer to the element within the A2 that is in the 
 *         indices (col, row)
 * Expects:
 *      * An existing A2 is entered in as the first parameter
 *      * The row value is positive and is less than the height of the UArray2
 *      * The col value is positive and is less than the width of the UArray2
 * Notes:
 *      Returns a pointer!
 *      * This function solely utilizes the UArray2_at function, and thus it
 *        initializes the following checked runtime errors through 
 *        that function:
 *              * UArray2 is null 
 *              * row value >= height or row value < 0
 *              * col value >= width or col value < 0
 ************************/
static A2Methods_Object *at(A2 array2, int col, int row)
{
        return UArray2_at(array2, col, row);
}

/***********UArray2_applyfun********
 * 
 * Declaration of the type of an apply function used by this methods suite's
 * mapping functions
 * Inputs:
 *              int col: the current col index
 *              int row: the current row index
 *              UArray2_T uarray2: the UArray2_T instance being mapped
 *              void *elem: A pointer to the current element 
 *                                    at pos (col, row) in the UArray2
 *              void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 * Returns: N/A
 * 
 * Notes: 
 *              This is a function declaration, which can be written for any 
 *              number of mapping applications so long as it matches this
 *              function declaration
 */
typedef void UArray2_applyfun(int col, int row, UArray2_T uarray2, 
                              void *elem, void *cl);


/**********map_row_major********
 *
 * Calls an apply function for each element in the argued A2, in order from 
 * low to high indices, mapping through each column in a row before moving 
 * to the next row
 * Inputs:
 *              A2Methods_UArray2 uarray2: A pointer to the A2 instance that 
 *                          the apply function will be called on 
 *                         
 *              A2Methods_applyfun apply: The function that will be applied to 
 *                          each element in uarray2 (parameters detailed above
 *                          with the typedef declaration of A2Methods_applyfun)
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
 *      Updates the uarray2 entered in as the first parameter 
 ************************/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/**********map_col_major********
 *
 * Calls an apply function for each element in the argued A2, in order from 
 * low to high indices, mapping through each row in a column before moving 
 * to the next column
 * Inputs:
 *              A2Methods_UArray2 uarray2: A pointer to the A2 instance that 
 *                          the apply function will be called on 
 *                         
 *              A2Methods_applyfun apply: The function that will be applied to 
 *                          each element in uarray2 (parameters detailed above
 *                          with the typedef declaration of A2Methods_applyfun)
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
 *      Updates the uarray2 entered in as the first parameter 
 ************************/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* 
* All functions from here onwards are written by COURSE STAFF
*/

/**********struct small_closure********
 *
 * The small apply functions closing argument
 ************************/
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/**********apply_small********
 *
 * An apply function for simple mapping procedures. This apply function gives
 * the user access to the element at the present iteration and the closing 
 * argument
 ************************/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/**********small_map_row_major********
 *
 * A mapping function that first indexes through a row's columns and then moves
 * on to the next row to do the same. This mapping function utilizes the small 
 * apply function, so it can only access a current iteration's element and the
 * closing argument. 
 ************************/
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/**********small_map_col_major********
 *
 * A mapping function that first indexes through column's rows and then moves 
 * on to the next column to do the same. This mapping function calls 
 * map_col_major with apply_small as its apply function, so it can only access
 * a current iteration's element and the closing argument.
 ************************/
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/*
 * This struct assigns each of the functions defined above to the corresponding
 * function pointers of an A2Methods_T struct. The NULL pointers correspond to
 * block major mapping functions, which are irrelevant for this implementation,
 * as it utilizes row-organized UArray2_T's
 */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,        /* map_row_major */
        map_col_major,        /* map_col_major */
        NULL,
        map_row_major,        /* map_default */
        small_map_row_major,  // small_map_row_major
        small_map_col_major,  // small_map_col_major
        NULL,
        small_map_row_major,  // small_map_default
};

/* finally the payoff: here is the exported pointer to the struct of this 
 * methods suite
 */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;