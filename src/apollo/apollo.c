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
#define TER_BLUE   "\033[34;1m"
#define TER_RESET  "\033[0m"


static flag_info_t flag_info = {
    .input_filepath = NULL,
    .output_filepath = "output",
    .only_asm = 0,
    .verbose_output = 0,
#ifdef __APPLE__
    .target = PLATFORM_MAC_ARM64
#elif __linux__
    .target = PLATFORM_LINUX_X64
#else
#error "unsupported platform"
#endif
};


/*
 *  Prints a little help message, you get this by running the compiler with the '-h' flag.
*/
static void __print_usage(void)
{
    printf("Usage: apollo [flags] <input filepath>\n");
    printf("Flags:\n");
    printf("        -h          Prints this help message.\n");
    printf("        -v          Verbose output.\n");
    printf("        -asm        Only generates the assembly.\n");
    printf("        -mac-arm64  Generates code for arm64 (m1) with macOS.\n");
    printf("        -linux-x64  Generates code for intel x64 with Linux.\n");
}


/*
 *  Parses a specific flag.
 *  @param argc:       The total amount of arguments.
 *  @param argv_index: The index of the current argument.
 *  @param argv:       The list with arguments.
 *  @param flag:       The current argument.
*/
static void __parse_flag(int argc, int *argv_index, char **argv, const char *flag)
{
    /* because we don't use these parameters yet. */
    assert(argv);
    assert(argc);
    assert(argv_index);

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

    else {
        fprintf(stderr, TER_RED "error: unkown flag '%s'\n" TER_RESET, flag);
        exit(1);
    }
}


/*
 *  Parses the command-line arguments.
 *  @param argc: The amount of arguments.
 *  @param argv: The list with arguments.
 *  @return:     The struct that contains all the info about the arguments.
*/
flag_info_t apollo_parse_arguments(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, TER_RED "error: no input file provided\n" TER_RESET);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            __parse_flag(argc, &i, argv, argv[i]);
        else
            flag_info.input_filepath = argv[1];
    }

    if (flag_info.input_filepath == NULL) {
        fprintf(stderr, TER_RED "error: no input filepath provided\n" TER_RESET);
        exit(1);
    }

    return flag_info;
}


/*
 *  A wrapper around printf, the string will only be printed if the verbose
 *  flag is set.
 *  @param fmt: The formatted string.
*/
void printf_verbose(const char *fmt, ...)
{
    if (flag_info.verbose_output) {
        va_list args;
        va_start(args, fmt);

        vprintf(fmt, args);

        va_end(args);
    }
}


/*
 *  Returns the string version of the target platform.
 *  @param target: The target platform.
 *  @return:       The target platform as a string.
*/
static const char *__platform_to_str(target_platforms_t target)
{
    switch (target) {
        case PLATFORM_MAC_ARM64: return "macOS-arm64";
        case PLATFORM_LINUX_X64: return "linux-x64";

        default: assert(0);
    }
}


/*
 *  Assembles the output assembly file, for macOS-arm64 platform we use the builtin assembler 'as'
 *  and 'ld' to link it. On linux-x64 we also use the builtin assembler GAS, and also 'ld' for linking.
*/
void apollo_assemble(void)
{
    printf_verbose("target platform: \033[34;1m%s\033[0m\n\n", __platform_to_str(flag_info.target));

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
        if (system("as -o output.o output.s") == 0) {
            printf_verbose(TER_GREEN "[verbose-mode]: as -o output.o output.s\n" TER_RESET);
        } else {
            printf_verbose(TER_RED "[verbose-mode]: as -felf64 -o output.o output.s\n" TER_RESET);
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
