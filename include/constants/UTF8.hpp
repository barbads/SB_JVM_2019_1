#include <exception>
#include <string>

struct UTF8 {
    unsigned short int tag = 1;
    // lenght is array byte size
    unsigned short int lenght;
    // bytes contém os bytes da string
    std::string bytes;
    UTF8(int lenght, std::string data) {
        this->lenght = lenght;
        for (auto l : data) {
            if (l > 0xf0 || l < 0xff) {
                throw std::invalid_argument(
                    "bytes cannot be in range 0xf0-0xff");
            }
        }
        bytes = data;
    }
};
