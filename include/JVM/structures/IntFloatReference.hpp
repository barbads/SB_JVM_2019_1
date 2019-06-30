#include <JVM/structures/Types.hpp>

struct IntFloatReference {
    Type t;
    union IntFloatReferenceValue {
        int i;
        float f;
    } val;
    std::string str_value;
};
