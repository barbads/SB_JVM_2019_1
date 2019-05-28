#include <MethodInfo.hpp>

MethodInfo::MethodInfo(std::ifstream *file, int linetableindex) {
    this->file       = file;
    mi               = std::vector<MethodInfoCte>();
    line_table_index = linetableindex;
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
            auto attribute_name_index =
                static_cast<unsigned short int>(getInfo(file, 2));
            auto attribute_lenght = getInfo(file, 4);
            auto attribute_info   = reinterpret_cast<unsigned char *>(
                getInfoRaw(file, attribute_lenght).data());
            ai[j] = AttributeInfo{attribute_name_index, attribute_lenght,
                                  attribute_info};
        }
        auto file_info = MethodInfoCte(access_flags, name_index,
                                       descriptor_index, attributes_count, ai);
        mi.push_back(file_info);
    }
}

AttributeInfo MethodInfo::readAttrCode() {
    /*auto attr_name_index = static_cast<unsigned short int>(getInfo(file, 2));
    auto attr_lenght     = static_cast<unsigned int>(getInfo(file, 4));
    auto max_stack       = static_cast<unsigned short int>(getInfo(file, 2));
    auto max_locals      = static_cast<unsigned short int>(getInfo(file, 2));
    auto code_lenght     = static_cast<unsigned int>(getInfo(file, 4));
    auto code            = getUTF8Data(file, code_lenght);
    auto et_length       = static_cast<unsigned short int>(getInfo(file, 2));
    exception *et        = new exception[et_length];

    for (int i = 0; i < et_length; i++) {
        et[i].start_pc   = static_cast<unsigned int>(getInfo(file, 2));
        et[i].end_pc     = static_cast<unsigned int>(getInfo(file, 2));
        et[i].handler_pc = static_cast<unsigned int>(getInfo(file, 2));
        et[i].catch_type = static_cast<unsigned int>(getInfo(file, 2));
    }

    auto attributes_count = static_cast<unsigned short int>(getInfo(file, 2));
    auto ai               = std::vector<AttributeInfo>();
    LineTableNumber ltn;
    auto hasLT = false;
    for (int i = 0; i < attributes_count; i++) {
        auto name_index = getInfo(file, 2);
        if (name_index == line_table_index) {
            hasLT                                       = true;
            unsigned int attribute_lenght               = getInfo(file, 4);
            unsigned short int line_number_table_length = getInfo(file, 2);
            auto linetable                              = LineTableNumber{
                static_cast<unsigned short int>(name_index),
                attribute_lenght,
                line_number_table_length,
            };
            linetable.line_table =
                std::map<unsigned short int, unsigned short int>();
            for (int j = 0; j < line_number_table_length; j++) {
                unsigned short int start_pc    = getInfo(file, 2);
                unsigned short int line_number = getInfo(file, 2);
                linetable.line_table.insert(
                    std::make_pair(start_pc, line_number));
            }
            ltn = linetable;
        } else {
            int attr_len = getInfo(file, 4);
            auto info    = reinterpret_cast<unsigned char *>(
                getInfoRaw(file, attr_len).data());
            auto insert_ai = AttributeInfo{
                static_cast<unsigned short int>(name_index),
                attr_len,
                info,
            };
            ai.push_back(insert_ai);
        }
    }
    AttributeCode ac{
        attr_name_index, attr_lenght, max_stack, max_locals,       code_lenght,
        &code[0],        et_length,   et,        attributes_count, ai,
        hasLT,           ltn,         "",
    };
    return ac;*/
}

std::vector<MethodInfoCte> *MethodInfo::getMethodInfo() { return &mi; }

void MethodInfo::showMI() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "               MethodInfo" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    for (auto elem : mi) {
        std::cout << "Name: " << elem.name << std::endl
                  << "Descriptor: " << elem.descriptor << std::endl
                  << "Access Flags: 0x" << std::setfill('0') << std::setw(4)
                  << std::hex << elem.access_flags << std::endl;
        for (int j = 0; j < elem.attributes_count; j++) {
            std::cout << "Nome do Attributo " << j + 1 << ": "
                      << elem.attributes[j].name << std::endl;
        }
        std::cout << std::endl;
        /*for (auto attr : elem.attributes) {

            std::cout << "  Attribute: " << attr.name << std::endl
                      << "    info: \n"
                      << "      index: " << attr.attribute_name_index
                      << std::endl
                      << "      length: " << attr.attribute_length << std::endl;
            auto code_lenght = attr.code_length;
            if (code_lenght) {
                std::cout << "      code: " << std::endl;
                std::cout << "        ";
                for (int j = 0; j < attr.code_length; j++) {
                    std::cout << (unsigned char)attr.code[j];
                }
                std::cout << std::endl;
            }
            auto exception_table_length = attr.exception_table_length;
            std::cout << "      exception table length: "
                      << exception_table_length << std::endl;

            if (exception_table_length) {
                for (int j = 0; j < exception_table_length; j++) {
                    std::cout << "      start_pc: "
                              << attr.exception_table[j].start_pc << std::endl;
                    std::cout
                        << "      end_pc: " << attr.exception_table[j].end_pc
                        << std::endl;
                    std::cout << "      handler_pc: "
                              << attr.exception_table[j].handler_pc
                              << std::endl;
                    std::cout << "      catch_type: "
                              << attr.exception_table[j].catch_type
                              << std::endl;
                }
            }
            if (attr.hasLineTable) {
                std::cout << "      LineNumberTable:" << std::endl;
                std::cout << "        Generic info:" << std::endl;
                std::cout << "          Attribute name index: cp_info #"
                          << attr.ltn.attribute_name_index << std::endl;
                std::cout << "          Attribute lenght: "
                          << attr.ltn.attribute_length << std::endl;
                std::cout << "        Specific info: " << std::endl;
                if (!attr.ltn.line_table.empty()) {
                    std::cout << "          start_pc    |    line_number"
                              << std::endl;
                    for (auto pair : attr.ltn.line_table) {
                        std::cout << "          " << pair.first;
                        for (int i = std::to_string(pair.first).size(); i < 12;
                             i++)
                            std::cout << " ";
                        std::cout << "|    " << pair.second << std::endl;
                    }
                }
            }
            std::cout << "      misc: " << std::endl
                      << "        Max stack depth: " << attr.max_stack
                      << std::endl;
            std::cout << "        Max local variables: " << attr.max_locals
                      << std::endl;
            std::cout << "        Code length: " << attr.code_length
                      << std::endl
                      << std::endl;
        }*/
    }
    std::cout << std::endl;
}