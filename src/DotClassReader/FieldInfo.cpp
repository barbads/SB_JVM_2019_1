#include <DotClassReader/FieldInfo.hpp>

FieldInfo::FieldInfo(std::ifstream *file) {
    this->file = file;
    fi         = std::vector<FieldInfoCte>();
}
void FieldInfo::seek() {
    field_count = getInfo(file, 2);
    for (int i = 0; i < field_count; i++) {
        auto access_flags     = getInfo(file, 2);
        auto name_index       = getInfo(file, 2);
        auto descriptor_index = getInfo(file, 2);
        auto attributes_count = getInfo(file, 2);
        AttributeInfo *ai     = new AttributeInfo[attributes_count];

        for (int j = 0; j < attributes_count; j++) {
            auto attribute_name_index =
                static_cast<unsigned short int>(getInfo(file, 2));
            auto attribute_lenght = getInfo(file, 4);
            auto attribute_info   = reinterpret_cast<unsigned char *>(
                getInfoRaw(file, attribute_lenght).data());
            ai[j] = AttributeInfo{
                attribute_name_index,
                attribute_lenght,
                attribute_info,
            };
        }
        auto field_info = FieldInfoCte(access_flags, name_index,
                                       descriptor_index, attributes_count, ai);
        fi.push_back(field_info);
    }
}

std::vector<FieldInfoCte> *FieldInfo::getFieldInfo() { return &fi; }

void FieldInfo::show() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "               FieldInfo" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    for (auto elem : fi) {
        std::cout << "Name: " << elem.name << std::endl
                  << "Descriptor: " << elem.descriptor << std::endl
                  << "Access Flags: 0x" << std::setfill('0') << std::setw(4)
                  << std::hex << elem.access_flags << std::endl;
        for (int j = 0; j < elem.attributes_count; j++) {
            std::cout << "Nome do Attributo " << j + 1 << ": "
                      << elem.attributes[j].name << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int FieldInfo::fiCount() { return field_count; }