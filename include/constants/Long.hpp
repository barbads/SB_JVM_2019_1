#include <string.h>
#include <vector>

/**
 * Represents the constant pool entry of the type CONSTANT_Long_info (tag = 5)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.5
 */
struct Long {
    unsigned short int tag = 5;
    unsigned char *high_bytes;
    unsigned char *low_bytes;
    long val;
    Long(std::vector<unsigned char> hbytes, std::vector<unsigned char> lbytes) {
        high_bytes = reinterpret_cast<unsigned char *>(hbytes.data());
        low_bytes  = reinterpret_cast<unsigned char *>(lbytes.data());
        unsigned char *fullbytes;
        strcpy((char *)fullbytes, (char *)high_bytes);
        strcat((char *)fullbytes, (char *)low_bytes);
        memcpy(&val, fullbytes, sizeof(long));
    };
    long getValue() { return val; };
};
