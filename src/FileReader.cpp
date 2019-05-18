#include <FileReader.hpp>
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

int FileReader::getInfo(std::ifstream *file, int offset) {
    // getInfo reads next {offset} bytes and returns it as an int
    // We use this to get indexes
    std::stringstream ss;
    char tag[offset];
    file->read(tag, offset);
    for (auto i = 0; i < offset; ++i) {
        ss << static_cast<int>(static_cast<unsigned char>(tag[i]));
    }
    return stoi(ss.str());
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