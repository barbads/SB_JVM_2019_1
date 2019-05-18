#include <constants/AttributeCode.hpp>
#include <string>

struct MethodInfoCte {
    unsigned short int access_flags;
    unsigned short int name_index;
    std::string name;
    std::string descriptor;
    unsigned short int descriptor_index;
    unsigned short int attributes_count;
    AttributeCode *attributes;
    MethodInfoCte(int af, int ni, int di, int ac, AttributeCode *attr) {
        access_flags     = af;
        name_index       = ni;
        descriptor_index = di;
        attributes_count = ac;
        attributes       = attr;
    }
};
