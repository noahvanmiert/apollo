#ifndef __VARIABLE_H_
#define __VARIABLE_H_

// TODO: implemented all types, uint8, uint16, ...

typedef enum {
    TYPE_UINT32,
    TYPE_UNKOWN
} data_type_t;


data_type_t get_type_from_str(const char *word);
const char *get_string_from_type(data_type_t type);
int get_type_size(data_type_t type);


#endif // __VARIABLE_H_
