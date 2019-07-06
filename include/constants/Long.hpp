#include <vector>

struct Long {
    unsigned short int tag = 5;
    unsigned char *high_bytes;
    unsigned char *low_bytes;
    long val;
    Long(std::vector<unsigned char> hbytes, std::vector<unsigned char> lbytes) {
        unsigned int high =
            (*(reinterpret_cast<unsigned char *>(hbytes.data())) << 8) & 0xF0;
        unsigned int low_bytes =
            *(reinterpret_cast<unsigned char *>(lbytes.data())) & 0x0F;
        unsigned char fullbytes = high | low_bytes;
        if (high_bytes == nullptr) {
            std::cout << "here" << std::endl;
        }
        // memcpy(&val, fullbytes, sizeof(long));
    };
    long getValue() { return val; };
};
