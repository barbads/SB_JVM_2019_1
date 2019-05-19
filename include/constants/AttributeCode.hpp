#ifndef _AttributeCode_H_
#define _AttributeCode_H_

#include <constants/AttributeInfo.hpp>
#include <constants/LineTableNumber.hpp>
#include <string>

struct exception {
    unsigned short int start_pc;
    unsigned short int end_pc;
    unsigned short int handler_pc;
    unsigned short int catch_type;
};

struct AttributeCode {
    unsigned short int attribute_name_index;
    unsigned int attribute_length;
    unsigned short int max_stack;
    unsigned short int max_locals;
    unsigned int code_length;
    char *code;
    unsigned short int exception_table_length;
    exception *exception_table;
    unsigned short attributes_count;
    std::vector<AttributeInfo> attributes;
    bool hasLineTable;
    LineTableNumber ltn;
    std::string name;
};

#endif
