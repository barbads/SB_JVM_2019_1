#include <Attributes.hpp>

Attributes::Attributes(std::ifstream *file) { this->file = file; }

void Attributes::seek() {
    attributes_count = getInfo(file, 2);
    for (int i = 0; i < attributes_count; i++) {
        auto attribute = AttributeClassFile{
            static_cast<unsigned short int>(getInfo(file, 2)),
            static_cast<unsigned int>(getInfo(file, 4)),
            static_cast<unsigned short int>(getInfo(file, 2)),
        };
        attr.push_back(attribute);
    }
}

std::vector<AttributeClassFile> *Attributes::getClassAttributes() {
    return &attr;
}

void Attributes::show() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "               Attributes" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    for (auto attribute : attr) {
        std::cout << "  Generic Info: " << std::endl;
        std::cout << "    Attribute name index: cp_info #"
                  << attribute.attribute_name_index << "  " << attribute.name
                  << std::endl;
        std::cout << "     Attribute lenght: " << attribute.attribute_length
                  << std::endl;
        std::cout << "  Specific info:" << std::endl;
        std::cout << "    Source file name index: cp_info #"
                  << attribute.sourcefile_index << "  " << attribute.sourcefile
                  << std::endl;
    }
    std::cout << std::endl;
}