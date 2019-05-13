#include <string>

struct Class {
    unsigned short int tag = 7;
    int name_index;
    Class(int index) : name_index(index){};
    // name_index is a valid index to constant_pool
    // e.g: Ljava/lang/Thread
};
