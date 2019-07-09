#include <vector>

/**
 * Represents the constant pool entry of the type CONSTANT_Long_info (tag = 5)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.5
 */
struct Long {
    unsigned short int tag = 5;
    std::string high_bytes;
    std::string low_bytes;
    unsigned char full_bytes[8];
    long val;
    Long(std::vector<unsigned char> hbytes, std::vector<unsigned char> lbytes) {
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
            full_bytes[7 - i] = lbytes[i - 4];
        }

        memcpy(&val, full_bytes, sizeof(long));
    };
    long getValue() { return val; };
};
