#ifndef _LineTableNumber_H_
#define _LineTableNumber_H_
#include <map>

struct LineTableNumber {
    unsigned short int attribute_name_index;
    unsigned int attribute_length;
    unsigned short int line_number_table_length;
    std::map<unsigned short int, unsigned short int> line_table;
};

#endif