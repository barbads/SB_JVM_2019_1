#include <JVM/structures/Types.hpp>

struct DoubleLong {
    Type t;
    union DoubleLongValue {
        double d;
        long l;
    } val;
};
