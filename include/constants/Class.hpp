#include <string>
/**
 * Represents the constant pool entry of the type CONSTANT_Class_info (tag = 7)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.1
 */
struct Class {
    unsigned short int tag = 7;
    int name_index; /// valid index to constant_pool e.g: Ljava/lang/Thread
    std::string name;
    Class(int index) : name_index(index){};
};
