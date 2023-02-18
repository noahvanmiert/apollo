/*
 *	Made by Noah Van Miert
 *	16/02/2023
*/

#include "variable.h"

#include <string.h>
#include <assert.h>


/*
 *  Returns the datatype corresponding to the string-version of the datatype.
 *  @param word: The word with the string-version of the datatype.
*/
data_type_t get_type_from_str(const char *word)
{
    if (strcmp(word, "uint32") == 0)
        return TYPE_UINT32;

    return TYPE_UNKOWN;
}


/*
 *  Returns the datatype corresponding with the AST. type.
 *  @param ast: The AST.
*/
data_type_t get_type_from_ast(ast_t *ast)
{
    switch (ast->type) {
        case AST_UINT32: return TYPE_UINT32;

        default: return TYPE_UNKOWN;
    }
}


/*
 *  Returns the string-version of a datatype.
 *  @param type: The datatype.
*/
const char *get_string_from_type(data_type_t type)
{
    switch (type) {
        case TYPE_UINT32: return "uint32";
        case TYPE_UNKOWN: return "unkown";

        default: assert(0);
    }
}


#define __TYPE_UINT32_SIZE 4


/*
 *  Returns the size of a datatype in bytes.
 *  @param type: The datatype.
*/
int get_type_size(data_type_t type)
{
    switch (type) {
        case TYPE_UINT32: return __TYPE_UINT32_SIZE;
        case TYPE_UNKOWN: return 0;

        default: assert(0);
    }
}
