#include <string>

struct UTF8 {
    unsigned short int tag = 1;
    unsigned short int lenght;
    std::string bytes;
    UTF8(int lenght, std::string data) {
        this->lenght = lenght;
        bytes        = data;
    }
};
