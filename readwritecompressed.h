#ifndef READWRITECOMPRESSED_INCLUDED
#define READWRITECOMPRESSED_INCLUDED

void print_to_stdout(UArray2b_T compressed_blocks);
UArray2b_T read_compressed_file(FILE *input);

#endif