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
        AttributeCode *ac     = new AttributeCode[attributes_count];

        for (int j = 0; j < attributes_count; j++) {
            auto attribute_info = readAttr();
            ac[j]               = attribute_info;
        }
        auto file_info = MethodInfoCte(access_flags, name_index,
                                       descriptor_index, attributes_count, ac);
        mi.push_back(file_info);
    }
}

AttributeCode MethodInfo::readAttr() {
    auto attr_name_index = getInfo(file, 2);
    auto attr_lenght     = getInfo(file, 4);
    auto max_stack       = getInfo(file, 2);
    auto max_locals      = getInfo(file, 2);
    auto code_lenght     = getInfo(file, 4);
    auto code =
        reinterpret_cast<unsigned char *>(getInfoRaw(file, code_lenght).data());
    std::string s(reinterpret_cast<char *>(code));
    auto et_length = getInfo(file, 2);
    exception *et  = new exception[et_length];
    for (int i = 0; i < et_length; i++) {
        et[i].start_pc   = getInfo(file, 2);
        et[i].end_pc     = getInfo(file, 2);
        et[i].handler_pc = getInfo(file, 2);
        et[i].catch_type = getInfo(file, 2);
    }
    auto attributes_count = getInfo(file, 2);
    AttributeInfo *ai     = new AttributeInfo[attributes_count];
    for (int i = 0; i < attributes_count; i++) {
        ai[i].attribute_name_index = getInfo(file, 2);
        ai[i].attribute_lenght     = getInfo(file, 4);
        ai[i].info                 = reinterpret_cast<unsigned char *>(
            getInfoRaw(file, ai[i].attribute_lenght).data());
    }
    AttributeCode ac{
        attr_name_index,  attr_lenght, max_stack,     max_locals,
        code_lenght,      code,        et_length,     et,
        attributes_count, ai,          std::string(),
    };
    return ac;
}

std::vector<MethodInfoCte> *MethodInfo::getMethodInfo() { return &mi; }

void MethodInfo::showMI() {
    std::cout << "MethodInfo" << std::endl;
    for (auto elem : mi) {
        std::cout << "method: " << elem.name << "\nDesc:" << elem.descriptor
                  << std::endl;
        for (int i = 0; i < elem.attributes_count; i++) {
            std::cout << "  Attribute: " << elem.attributes[i].name << std::endl
                      << "    info: \n"
                      << "      index: "
                      << elem.attributes[i].attribute_name_index << std::endl
                      << "      length: " << elem.attributes[i].attribute_length
                      << std::endl;
            auto code_lenght = elem.attributes[i].code_length;
            if (code_lenght) {
                std::cout << "      code: " << std::endl;
                std::cout << "        " << elem.attributes[i].code << std::endl;
            }
            auto exception_table_length =
                elem.attributes[i].exception_table_length;
            std::cout << "      exception table length: "
                      << exception_table_length << std::endl;

            if (exception_table_length) {
                for (int j = 0; j < exception_table_length; j++) {
                    std::cout << "      start_pc: "
                              << elem.attributes[i].exception_table[j].start_pc
                              << std::endl;
                    std::cout << "      end_pc: "
                              << elem.attributes[i].exception_table[j].end_pc
                              << std::endl;
                    std::cout
                        << "      handler_pc: "
                        << elem.attributes[i].exception_table[j].handler_pc
                        << std::endl;
                    std::cout
                        << "      catch_type: "
                        << elem.attributes[i].exception_table[j].catch_type
                        << std::endl;
                }
            }
            std::cout << "      misc: " << std::endl
                      << "        Max stack depth: "
                      << elem.attributes[i].max_stack << std::endl;
            std::cout << "        Max local variables: "
                      << elem.attributes[i].max_locals << std::endl;
            std::cout << "        Max local variables: "
                      << elem.attributes[i].max_locals << std::endl;
            std::cout << "        Code length: "
                      << elem.attributes[i].code_length << std::endl
                      << std::endl;
        }
    }
}