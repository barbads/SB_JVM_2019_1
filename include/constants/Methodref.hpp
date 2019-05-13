#include <sstream>
#include <string>

struct Methodref {
    unsigned short int tag = 10;
    // class_index is a valid index to constant_pool
    unsigned short int class_index;
    // name_type_index is a valid index to constant_pool
    // if starts with <, then it must be <init>
    // representing an instance init method
    unsigned short int name_type_index;
    Methodref(int class_index, int name_type_index) {
        this->class_index     = class_index;
        this->name_type_index = name_type_index;
    }
};
