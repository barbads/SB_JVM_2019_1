#include <string>

struct NameAndType {
    unsigned short int tag = 12;
    unsigned short int name_index;
    std::string name;
    // name_index is a valid index to constant_pool
    // represents a simple field name or method or special method name <init>
    unsigned short int descriptor_index;
    std::string descriptor;
    // descriptor_index is a valid index to constant_pool
    // represents a valid field or method descriptor
    NameAndType(int nidx, int didx)
        : name_index(nidx), descriptor_index(didx){};
};
