#include <string>

struct NameAndType {
    unsigned short int tag = 12;
    std::string name_index;
    // name_index is a valid index to constant_pool
    // represents a simple field name or method or special method name <init>
    std::string descriptor_index;
    // descriptor_index is a valid index to constant_pool
    // represents a valid field or method descriptor
};
