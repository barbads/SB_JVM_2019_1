#include <string>

/**
 * Represents the constant pool entry of the type
 * CONSTANT_InterfaceMethodref_info (tag = 11).
 */
struct InterfaceMethodref {
    unsigned short int tag = 11;
    int class_index; /// valid index to constant_pool
    std::string class_name;
    int name_type_index; /// valid index to constant_pool
    std::string name_and_type;
    InterfaceMethodref(int cidx, int ntidx)
        : class_index(cidx), name_type_index(ntidx){};
};
