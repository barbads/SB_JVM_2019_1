#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

struct Double {
    unsigned short int tag = 6;
    std::string high_bytes;
    std::string low_bytes;
    unsigned char full_bytes[8];
    double li;
    Double(std::vector<unsigned char> hbytes,
           std::vector<unsigned char> lbytes) {
        std::stringstream ss;
        for (auto b : hbytes) {
            ss << std::hex << static_cast<int>(b);
            std::cout << std::hex << static_cast<int>(b);
        }
        std::cout << std::endl;
        high_bytes = ss.str();
        ss         = std::stringstream();
        for (auto b : lbytes) {
            ss << std::hex << static_cast<int>(b);
        }
        low_bytes      = ss.str();
        unsigned int i = 0;
        for (; i < 4; i++) {
            full_bytes[7 - i] = hbytes[i];
        }
        for (; i < 8; i++) {
            full_bytes[7 - i] = hbytes[i];
        }

        memcpy(&li, full_bytes, sizeof(double));
    };
    double getValue() { return li; };
};