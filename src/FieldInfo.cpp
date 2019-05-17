#include <FieldInfo.hpp>

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
            ai[j] = *reinterpret_cast<AttributeInfo *>(
                getInfoRaw(file, sizeof(AttributeInfo)).data());
        }
        auto file_info = FieldInfoCte(access_flags, name_index,
                                      descriptor_index, attributes_count, ai);
        fi.push_back(file_info);
    }
}

std::vector<FieldInfoCte> *FieldInfo::getFieldInfo() { return &fi; }

void FieldInfo::showFI() {
    std::cout << "FieldInfo" << std::endl;
    for (auto elem : fi) {
        std::cout << "Field: " << elem.name << "\nDesc:" << elem.descriptor
                  << std::endl
                  << std::endl;
    }
}