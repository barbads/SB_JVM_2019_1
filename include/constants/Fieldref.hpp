#include <string>

struct Fieldref {
    unsigned short int tag = 9;
    std::string class_index;
    // class_index is a valid index to constant_pool table
    // represents class's full name or interface
    // containing this field declaration
    std::string name_type_index;
    // name_type_index is a valid index to constant_pool
    // cte name and type indicating field's name and descriptor
};
