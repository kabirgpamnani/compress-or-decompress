/*
 *     uarray2.h
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Interface for 2D Unboxed Arrays 
 */


#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED
#define T UArray2_T
typedef struct T *T;


extern T UArray2_new(int width, int height, int size);
extern void UArray2_free(T *uarray2);
extern int UArray2_width(T uarray2);
extern int UArray2_height(T uarray2);
extern int UArray2_size (T uarray2);
extern void *UArray2_at(T uarray2, int col, int row);
extern void UArray2_map_row_major(T uarray2, void apply(int col, int row, 
                            T uarray2, void *element_at, void *cl), void *cl);
extern void UArray2_map_col_major(T uarray2, void apply(int col, int row, 
                            T uarray2, void *element_at, void *cl), void *cl);


#undef T
#endif





