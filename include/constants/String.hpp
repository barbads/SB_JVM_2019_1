#include <string>

/**
 * Represents the constant pool entry of the type CONSTANT_String_info (tag = 8)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.3
 */
struct String {
    unsigned short int tag = 8;
    int string_index; /// valid index to constant_pool containing a UTF8_info
    std::string string;
    String(int sidx) : string_index(sidx){};
};
