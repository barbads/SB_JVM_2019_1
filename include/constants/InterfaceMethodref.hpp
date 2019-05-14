#include <string>

struct InterfaceMethodref {
    unsigned short int tag = 11;
    // class_index is a valid index to constant_pool
    int class_index;
    std::string class_name;
    // name_and_type_index is a valid index to constant_pool
    int name_type_index;
    std::string name_and_type;
    InterfaceMethodref(int cidx, int ntidx)
        : class_index(cidx), name_type_index(ntidx){};
};
