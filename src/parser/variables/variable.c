#include "variable.h"

#include <string.h>
#include <assert.h>


data_type_t get_type_from_str(const char *word)
{
    if (strcmp(word, "uint32") == 0)
        return TYPE_UINT32;

    return TYPE_UNKOWN;
}


const char *get_string_from_type(data_type_t type)
{
    switch (type) {
        case TYPE_UINT32: return "uint32";
        case TYPE_UNKOWN: return "unkown";

        default: assert(0);
    }
}


#define __TYPE_UINT32_SIZE 4

int get_type_size(data_type_t type)
{
    switch (type) {
        case TYPE_UINT32: return __TYPE_UINT32_SIZE;
        case TYPE_UNKOWN: return 0;

        default: assert(0);
    }
}