#include <string>

/**
 * Represents the constant pool entry of the type CONSTANT_NameAndType_info (tag
 * = 12) more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.6
 */
struct NameAndType {
    unsigned short int tag = 12;
    unsigned short int
        name_index; /** valid index to constant_pool. represents a simple field
                       name or method or special method name <init> */
    std::string name;
    unsigned short int
        descriptor_index; /** valid index to constant_pool. Represents a valid
                            field or method descriptor */
    std::string descriptor;
    NameAndType(int nidx, int didx)
        : name_index(nidx), descriptor_index(didx){};
};
