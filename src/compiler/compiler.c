#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>


static const char *nasm_setup_code = ".global _start\n"
                                     ".align 2\n"
                                     "_start:\n"
                                     //"\tcall main\n"
                                     "\tmov X0, #0\n"
                                     "\tmov X16, 1\n"
                                     "\tsvc #0x80\n";


void compiler_write_asm(const char *filepath)
{
    FILE *fptr = fopen(filepath, "w");

    if (!fptr) {
        fprintf(stderr, "error: could not open file");
        exit(1);
    }

    fputs(nasm_setup_code, fptr);
    fclose(fptr);
}
