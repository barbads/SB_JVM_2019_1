#include <string>

struct Class {
    unsigned short int tag = 7;
    std::string name_index;
    // name_index is a valid index to constant_pool
    // e.g: Ljava/lang/Thread
};
