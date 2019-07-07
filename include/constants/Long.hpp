#include <vector>

/**
 * Represents the constant pool entry of the type CONSTANT_Long_info (tag = 5)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.5
 */
struct Long {
    unsigned short int tag = 5;
    long val;
    Long(unsigned int hbytes, unsigned int lbytes) {
        val = ((long)hbytes << 32) | lbytes;
    };
    long getValue() { return val; };
};
