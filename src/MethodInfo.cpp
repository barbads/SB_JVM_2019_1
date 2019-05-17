#include <MethodInfo.hpp>

MethodInfo::MethodInfo(std::ifstream *file) {
    this->file = file;
    mi         = std::vector<MethodInfoCte>();
}
void MethodInfo::seek() {
    method_count = getInfo(file, 2);

    for (int i = 0; i < method_count; i++) {
        auto access_flags     = getInfo(file, 2);
        auto name_index       = getInfo(file, 2);
        auto descriptor_index = getInfo(file, 2);
        auto attributes_count = getInfo(file, 2);
        AttributeInfo *ai     = new AttributeInfo[attributes_count];

        for (int j = 0; j < attributes_count; j++) {
            ai[j] = *reinterpret_cast<AttributeInfo *>(
                getInfoRaw(file, sizeof(AttributeInfo)).data());
        }
        auto file_info = MethodInfoCte(access_flags, name_index,
                                       descriptor_index, attributes_count, ai);
        mi.push_back(file_info);
    }
}

std::vector<MethodInfoCte> *MethodInfo::getMethodInfo() { return &mi; }

void MethodInfo::showMI() {
    std::cout << "MethodInfo" << std::endl;
    for (auto elem : mi) {
        std::cout << "method: " << elem.name << "\nDesc:" << elem.descriptor
                  << std::endl
                  << std::endl;
    }
}