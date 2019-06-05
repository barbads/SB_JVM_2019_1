#include <FileReader.hpp>
#include <iostream>
#include <sstream>
#include <string.h>

int FileReader::getTag(std::ifstream *file) {
    // getTag reads next byte in file and returns it as int
    std::stringstream ss;
    char tag[1];
    file->read(tag, 1);
    ss << static_cast<int>(static_cast<unsigned char>(tag[0]));
    return stoi(ss.str());
}

unsigned int FileReader::getInfo(std::ifstream *file, int offset) {
    // getInfo reads next {offset} bytes and returns it as an int
    // We use this to get indexes
    if ((offset % 2 != 0) || offset > 4) {
        throw std::invalid_argument("getInto only accepts 2 or 4");
    }
    char tag[4] = {0, 0, 0, 0};
    file->read(tag, offset);
    unsigned short int usi;
    unsigned int ui;
    unsigned int retval = 0;
    switch (offset) {
    case 2: {
        union int_bytes {
            unsigned char buf[2];
            unsigned short int number;
        } integer_bytes;
        for (int i = 0; i < offset; i++)
            integer_bytes.buf[i] = tag[1 - i];
        retval = static_cast<unsigned int>(integer_bytes.number);
    } break;
    case 4: {
        union int_bytes {
            unsigned char buf[4];
            unsigned int number;
        } integer_bytes;
        for (int i = 0; i < offset; i++)
            integer_bytes.buf[i] = tag[3 - i];
        retval = integer_bytes.number;
    } break;
    default:
        std::invalid_argument(
            "Only 2 or 4 bytes are available for this function");
        break;
    }
    return retval;
}

std::string FileReader::getInfoHex(std::ifstream *file, int offset) {
    // getInfo reads next {offset} bytes and returns it as an int
    // We use this to get indexes
    std::stringstream ss;
    char tag[offset];
    file->read(tag, offset);
    for (auto i = 0; i < offset; ++i) {
        ss << std::hex << static_cast<int>(static_cast<unsigned char>(tag[i]));
    }
    return ss.str();
}

std::vector<unsigned char> FileReader::getInfoRaw(std::ifstream *file,
                                                  int offset) {
    // getInfo reads next {offset} bytes and returns it as a vector<uchar>
    // we use it to get float, doubles and longs
    char tag[offset];
    unsigned char retval[offset];
    file->read(tag, offset);
    memcpy(retval, tag, offset);
    std::vector<unsigned char> ucharvec(retval, retval + offset);
    return ucharvec;
}

std::string FileReader::getUTF8Data(std::ifstream *file, int lenght) {
    // getUTF8Data read next {lenght} bytes and returns it as a string
    std::stringstream ss;
    char tag[lenght];
    file->read(tag, lenght);
    for (auto i = 0; i < lenght; ++i) {
        ss << (static_cast<unsigned char>(tag[i]));
    }
    return ss.str();
}