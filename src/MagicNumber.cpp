#include <MagicNumber.hpp>

MagicNumber::MagicNumber(std::ifstream *file) {
    // file is a pointer to an opened .class file
    this->file  = file;
    magicNumber = "";
}

std::string MagicNumber::getMagicNumber() {
    if (magicNumber == "") {
        file->seekg(0);
        std::stringstream ss;
        char magic[4];
        file->read(magic, 4);
        for (auto i = 0; i < 4; i++) {
            ss << std::hex
               << static_cast<int>(static_cast<unsigned char>(magic[i]));
        }
        magicNumber = ss.str();
    }

    if (magicNumber != "cafebabe") {
        throw std::range_error(
            "Wrong file position, could not read magic number properly");
    }
    return magicNumber;
}