#include <ClassFile.hpp>

ClassFile::ClassFile(std::ifstream *file) {
    this->file = file;
    this->cp   = new ConstantPool(this->file);
}

std::string ClassFile::getMagicNumber() {
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
    version    = major + "." + minor;
    cp->seek();
    access_flags         = getInfoHex(file, 2);
    this_class           = getInfo(file, 2);
    super_class          = getInfo(file, 2);
    auto Interface_count = getInfo(file, 2);
    if (Interface_count > 0) {
        getInfo(file, 2 * Interface_count);
    }

    fi = new FieldInfo(file);
    fi->seek();
    auto field_info = fi->getFieldInfo();
    for (auto &field : *field_info) {
        field.name       = cp->getNameByIndex(field.name_index);
        field.descriptor = cp->getNameByIndex(field.descriptor_index);
    }
    fi->showFI();

    std::cout << "Get Method" << std::endl;
    mi = new MethodInfo(file);
    mi->seek();
    auto method_info = mi->getMethodInfo();

    for (auto &method : *method_info) {
        method.name       = cp->getNameByIndex(method.name_index);
        method.descriptor = cp->getNameByIndex(method.descriptor_index);
    }
    mi->showMI();
    // add additional steps here
}