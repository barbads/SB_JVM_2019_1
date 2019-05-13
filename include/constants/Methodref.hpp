#include <sstream>
#include <string>

struct Methodref {
    unsigned short int tag = 10;
    unsigned short int class_index;
    unsigned short int name_type_index;
    Methodref(int class_index, int name_type_index) {
        this->class_index     = class_index;
        this->name_type_index = name_type_index;
    }
};
