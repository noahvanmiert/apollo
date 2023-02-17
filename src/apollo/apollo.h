/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#ifndef __APOLLO_H_
#define __APOLLO_H_


typedef enum {
    PLATFORM_MAC_ARM64,
    PLATFORM_LINUX_X64
} target_platforms_t;


typedef struct {
    const char *input_filepath;
    const char *output_filepath;
    int only_asm;
    int verbose_output;
    target_platforms_t target;
} flag_info_t;


flag_info_t apollo_parse_arguments(int argc, char **argv);
void apollo_assemble(void);


#endif // __APOLLO_H_
