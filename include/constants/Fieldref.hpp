#include <string>

/**
 * Represents the constant pool entry of the type CONSTANT_Fieldref_info (tag =
 * 9) more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.2
 */
struct Fieldref {
    unsigned short int tag = 9;
    unsigned short int
        class_index; /** valid index to constant_pool table. Represents class's
                        full name or interface */
    std::string class_name;
    unsigned short int name_type_index; /// valid index to constant_pool
    std::string name_and_type;
    Fieldref(int classidx, int ntidx)
        : class_index(classidx), name_type_index(ntidx){};
};
