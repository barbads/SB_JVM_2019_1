#include <vector>

struct Long {
    unsigned short int tag = 5;
    long val;
    Long(unsigned int hbytes, unsigned int lbytes) {
        val = ((long)hbytes << 32) | lbytes;
    };
    long getValue() { return val; };
};
