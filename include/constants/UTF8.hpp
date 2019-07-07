#include <exception>
#include <string>

/**
 * Represents the constant pool entry of the type CONSTANT_Utf8_info (tag = 1)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.7
 */
struct UTF8 {
    unsigned short int tag = 1;
    unsigned short int lenght; /// array byte size
    std::string bytes;         /// contains teh bytes of the string
    UTF8(int lenght, std::string data) {
        this->lenght = lenght;
        for (auto l : data) {
            if (l > 0xf0 && l < 0xff) {
                throw std::invalid_argument(
                    "bytes cannot be in range 0xf0-0xff");
            }
        }
        bytes = data;
    }
};
