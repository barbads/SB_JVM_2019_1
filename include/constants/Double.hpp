#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>

/**
 * Represents the constant pool entry of the type CONSTANT_Double_info (tag = 6)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.5
 */
struct Double {
    unsigned short int tag = 6;
    std::string high_bytes;
    std::string low_bytes;
    unsigned char full_bytes[32];
    double li;
    Double(std::vector<unsigned char> hbytes,
           std::vector<unsigned char> lbytes) {
        std::stringstream ss;
        for (auto b : hbytes) {
            ss << std::hex << static_cast<int>(b);
        }
        high_bytes = ss.str();
        ss         = std::stringstream();
        for (auto b : lbytes) {
            ss << std::hex << static_cast<int>(b);
        }
        low_bytes      = ss.str();
        auto high_byte = reinterpret_cast<unsigned char *>(hbytes.data());
        auto low_byte  = reinterpret_cast<unsigned char *>(lbytes.data());
        strcpy((char *)full_bytes, (char *)high_byte);
        strcat((char *)full_bytes, (char *)low_byte);
        std::string reverse_string((char *)full_bytes);

        std::reverse(reverse_string.begin(), reverse_string.end());
        strcpy((char *)full_bytes, reverse_string.c_str());
        memcpy(&li, full_bytes, sizeof(double));
    };
    double getValue() { return li; };
};