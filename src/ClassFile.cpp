#include <ClassFile.hpp>

ClassFile::ClassFile(std::ifstream *file) {
    this->file = file;
    this->cp   = new ConstantPool(this->file);
}

std::string ClassFile::getMagicNumber() {
    file->seekg(0);
    file->seekg(0);
    std::stringstream ss;
    char magic[4];
    file->read(magic, 4);
    for (auto i = 0; i < 4; i++) {
        ss << std::hex
           << static_cast<int>(static_cast<unsigned char>(magic[i]));
    }

    return ss.str();
}
void ClassFile::parse() {
    magic = getMagicNumber();
    if (magic != "cafebabe") {
        throw std::range_error(
            "Wrong file position, could not read magic number properly");
    }
    auto minor = getInfo(file, 2);
    auto major = getInfo(file, 2);
    cp->seek();
    // add additional steps here
}