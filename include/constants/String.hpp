#include <string>

struct String {
    unsigned short int tag = 8;
    // string_index is a valid index to constant_pool containing a UTF8_info
    int string_index;
    std::string string;
    String(int sidx) : string_index(sidx){};
};
