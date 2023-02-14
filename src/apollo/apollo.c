/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#include "apollo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>


#define TER_GREEN  "\033[32;1m"
#define TER_RED    "\033[31;1m"
#define TER_RESET  "\033[0m"


static flag_info_t flag_info = {
    .input_filepath = NULL,
    .output_filepath = "output",
    .only_asm = 0,
    .verbose_output = 0,
    .target = PLATFORM_MAC_ARM64
};


static void __print_usage(void)
{
    printf("Usage: apollo [flags] <input filepath>\n");
    printf("Flags:\n");
    printf("        -h          Prints this help message.\n");
    printf("        -v          Verbose output.\n");
    printf("        -asm        Only generates the assembly.\n");
    printf("        -mac-arm64  Generates code for arm64 (m1) with macOS.\n");
    printf("        -linux-x64  Generates code for intel x64 with Linux.\n");
    printf("        -o <name>   Sets the name of the output files to the given name.\n");
}


static void __parse_flag(int argc, int *argv_index, char **argv, const char *flag)
{
    if (strcmp(flag, "-h") == 0) {
        __print_usage();
        exit(0);
    }

    if (strcmp(flag, "-asm") == 0)
        flag_info.only_asm = 1;

    else if (strcmp(flag, "-v") == 0)
        flag_info.verbose_output = 1;

    else if (strcmp(flag, "-mac-arm64") == 0)
        flag_info.target = PLATFORM_MAC_ARM64;

    else if (strcmp(flag, "-linux-x64") == 0)
        flag_info.target = PLATFORM_LINUX_X64;

    else if (strcmp(flag, "-o") == 0) {
        assert(0 && "not implemented yet");

        if (argc < ++(*argv_index)) {
            fprintf(stderr, "\033[31;1merror: no output filepath provided after '-o' flag\n");
            exit(1);
        }

        flag_info.output_filepath = argv[*argv_index];
    }
}


flag_info_t apollo_parse_arguments(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "\033[31;1merror: no input file provided\033[0m\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            __parse_flag(argc, &i, argv, argv[i]);
        else
            flag_info.input_filepath = argv[1];
    }

    if (flag_info.input_filepath == NULL) {
        fprintf(stderr, "\033[31;1merror: no input filepath provided\033[0m\n");
        exit(1);
    }

    return flag_info;
}


void printf_verbose(const char *fmt, ...)
{
    if (flag_info.verbose_output) {
        va_list args;
        va_start(args, fmt);

        vprintf(fmt, args);

        va_end(args);
    }
}


static const char *__platform_to_str(target_platforms_t target)
{
    switch (target) {
        case PLATFORM_MAC_ARM64: return "macOS-arm64";
        case PLATFORM_LINUX_X64: return "linux-x64";

        default: assert(0);
    }
}


void apollo_assemble(void)
{
    printf_verbose("target platform: %s\n\n", __platform_to_str(flag_info.target));

    if (flag_info.target == PLATFORM_MAC_ARM64) {
        /* compile assembly */
        if (system("as -arch arm64 -o output.o output.s") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: as -arch arm64 -o output.o output.s\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: as -arch arm64 -o output.o output.s\n" TER_RESET);
            exit(1);
        }


        if (system("rm output.s") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: rm output.s\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: rm output.s\n" TER_RESET);
            exit(1);
        }


        /* link the object files */
        if (system("ld -o output output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: ld -o output output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: ld -o output output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64\n" TER_RESET);
            exit(1);
        }


        if (system("rm output.o") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: rm output.o\n\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: rm output.o\n" TER_RESET);
            exit(1);
        }
    }

    else if (flag_info.target == PLATFORM_LINUX_X64) {
        /* compile assembly */
        if (system("nasm -felf64 -o output.o output.s") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: nasm -felf64 -o output.o output.s\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: nasm -felf64 -o output.o output.s\n" TER_RESET);
            exit(1);
        }


        if (system("rm output.s") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: rm output.s\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: rm output.s\n" TER_RESET);
            exit(1);
        }


        /* link the object files */
        if (system("ld output.o -o output") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: ld output.o -o output\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: ld output.o -o output\n" TER_RESET);
            exit(1);
        }


        if (system("rm output.o") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: rm output.o\n\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: rm output.o\n" TER_RESET);
            exit(1);
        }
    }
}
