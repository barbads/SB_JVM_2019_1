#include <sstream>
#include <string>

/**
 * Represents the constant pool entry of the type CONSTANT_Methodref_info (tag =
 * 10). if starts with <, then it must be <init> representing an instance init
 * method.
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.2
 */
struct Methodref {
    unsigned short int tag = 10;

    unsigned short int class_index; /// valid index to constant_pool of type
                                    /// CONSTANT_Class_info.
    std::string class_name;
    unsigned short int name_type_index; /// valid index to constant_pool of type
                                        /// CONSTANT_NameAndType_info.
    std::string name_and_type;
    Methodref(int class_index, int name_type_index) {
        this->class_index     = class_index;
        this->name_type_index = name_type_index;
    }
};
