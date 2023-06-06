###########################################################################
#  *                          Makefile
#  *
#  *     Assignment: arith
#  *     Authors:    Kabir Pamnani & Isaac Monheit 
#  *     Date:       March 9th, 2023 
#  *
#  *      * Makefile for the arith homework assignment for cs40 spring 2023
#  *
#  *     Summary:
#  *      compiles all our source code and produces a 40image executable binary
#  *		and also a bitpack.o relocatable object
#  * 
#  *     Notes:
#  *   - adpated from the Makefile for locality (hw3)
###########################################################################

############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
# CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)
# 
# For this assignment, we have to change things a little.  We need
# to use the GNU 99 standard to get the right items in time.h for the
# the timing support to compile.
# 
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64 

# Libraries needed for linking
# All programs cii40 (Hanson binaries) and *may* need -lm (math)
# 40locality is a catch-all for this assignment, netpbm is needed for pnm
# rt is for the "real time" timing library, which contains the clock support
LDLIBS = -l40locality -lnetpbm -lcii40 -lm -larith40

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: 40image-6


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)
40image-6: 40image.o compress40.o rgbcomponent.o compress2x2.o quantization.o \
	     readwritecompressed.o uarray2.o uarray2b.o a2plain.o a2blocked.o \
		 bitpack.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f 40image *.o

