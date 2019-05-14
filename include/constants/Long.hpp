#include <string.h>
#include <vector>

struct Long {
    unsigned short int tag = 5;
    unsigned char *high_bytes;
    unsigned char *low_bytes;
    Long(std::vector<unsigned char> hbytes, std::vector<unsigned char> lbytes) {
        high_bytes = reinterpret_cast<unsigned char *>(hbytes.data());
        low_bytes  = reinterpret_cast<unsigned char *>(lbytes.data());
    };
    long getValue() {
        unsigned char *fullbytes;
        strcpy((char *)fullbytes, (char *)high_bytes);
        strcat((char *)fullbytes, (char *)low_bytes);
        long val;
        memcpy(&val, fullbytes, sizeof(long));
        return val;
    };
};
