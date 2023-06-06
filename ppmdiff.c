#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "assert.h"
#include "math.h"

int main(int argc, char *argv[]) 
{
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);
        
        if (argc != 3) {
                fprintf(stderr, "Needs to have 2 files inputted, or 1 file and a '-' character");
                exit(EXIT_FAILURE);
        }
        
        FILE *file_one;
        FILE *file_two;
        bool use_stdin = false;
        
        if (strcmp(argv[1], "-") == 0) {
                file_one = stdin;
                use_stdin = true;
        } else {
                file_one = fopen(argv[1], "r");
        }

        if (strcmp(argv[2], "-") == 0) {
                if (use_stdin) {
                        fprintf(stderr, "Can't have both file inputs be through stdin");
                        exit(EXIT_FAILURE);
                }
                file_two = stdin;
        } else {
                file_two = fopen(argv[2], "r");
        }

        assert(file_one != NULL);
        assert(file_two != NULL);
                
        // what to include for methods argument
        Pnm_ppm I = Pnm_ppmread(file_one, methods);
        Pnm_ppm I1 = Pnm_ppmread(file_two, methods);

        
        if (((abs((int)I->height - (int)I1->height)) > 1) || ((abs((int)I->width - (int)I1->width)) > 1)) {
                fprintf(stderr, "Height or width is greater than 1");
                printf("1.0");
        } 

        printf("passed all the checks\n");


        /* the calculation */

        
        int smallest_width = (fmin((int)I->width, (int)I1->width));
        int smallest_height = (fmin((int)I->height, (int)I1->height));
        float sum = 0;

        for (int i = 0; i < smallest_width; i++) {
                for (int j = 0; j < smallest_height; j++) {
                        Pnm_rgb rgb1 = I->methods->at(I->pixels, i, j);
                        Pnm_rgb rgb2 = I1->methods->at(I1->pixels, i, j);

                        float red1 = (float)rgb1->red / (float)I->denominator;
                        float red2 = (float)rgb2->red / (float)I1->denominator;

                        float blue1 = (float)rgb1->blue / (float)I->denominator;
                        float blue2 = (float)rgb2->blue / (float)I1->denominator;

                        float green1 = (float)rgb1->green / (float)I->denominator;
                        float green2 = (float)rgb2->green / (float)I1->denominator;

                        sum += (((red1 - red2)*(red1 - red2)) + ((blue1 - blue2)*(blue1 - blue2)) + ((green1 - green2)*(green1 - green2)));
                }
        }

        // fprintf(stderr, "sum: %f\n", sum);

        // how to test this more? we only have flowers.ppm and flowers_new.ppm
        float E = sqrt(sum / (3 * smallest_width * smallest_height));

        printf("%.4f\n", E);


        fclose(file_one);
        fclose(file_two);

        exit(EXIT_SUCCESS);
}

