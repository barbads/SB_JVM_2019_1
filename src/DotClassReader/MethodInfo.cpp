#include <DotClassReader/MethodInfo.hpp>
#include <sstream>
#include <system_error>

MethodInfo::MethodInfo(std::ifstream *file, ConstantPool *cp) {
    this->file       = file;
    this->cp         = cp;
    mi               = std::vector<MethodInfoCte>();
    line_table_index = cp->getLineTableIndex();
    code_index       = cp->getCodeIndex();
    access_flags     = std::map<int, std::string>{
        {0x01, "acc_public"},       {0x02, "acc_private"},
        {0x4, "acc_protected"},     {0x08, "acc_static"},
        {0x09, "public static"},    {0x10, "acc_final"},
        {0x20, "acc_synchronized"}, {0x400, "acc_abstract"},
        {0x800, "acc_strict"},
    };
    instruction_list = std::map<int, std::string>{
        {0x32, "aaload"},        {0x53, "aastore"},
        {0x01, "aconst_null"},   {0x19, "aload"},
        {0x2a, "aload_0"},       {0x2b, "aload_1"},
        {0x2c, "aload_2"},       {0x2d, "aload_3"},
        {0xbd, "anearray"},      {0xb0, "areturn"},
        {0xbe, "arraylength"},   {0x3a, "astore"},
        {0x4b, "astore_0"},      {0xae, "freturn"},
        {0x4c, "astore_1"},      {0x4d, "astore_2"},
        {0x4e, "astore_3"},      {0xbf, "athrow"},
        {0x33, "baload"},        {0x54, "bastore"},
        {0x10, "bipush"},        {0xca, "breakpoint"},
        {0x34, "caload"},        {0x55, "castore"},
        {0xc0, "checkcast"},     {0x90, "d2f"},
        {0x8e, "d2i"},           {0x8f, "d2l"},
        {0x63, "dadd"},          {0x31, "daload"},
        {0x52, "dastore"},       {0x98, "dcmpg"},
        {0x97, "dcmpl"},         {0x0e, "dconst_0"},
        {0x0f, "dconst_1"},      {0x6f, "ddiv"},
        {0x18, "dload"},         {0x26, "dload_0"},
        {0x27, "dload_1"},       {0x28, "dload_2"},
        {0x29, "dload_3"},       {0x6b, "dmul"},
        {0x77, "dneg"},          {0x73, "drem"},
        {0xaf, "dreturn"},       {0x39, "dstore"},
        {0x47, "dstore_0"},      {0x48, "dstore_1"},
        {0x4a, "dstore_3"},      {0x67, "dsub"},
        {0x59, "dup"},           {0x5a, "dup_x1"},
        {0x5b, "dup_x2"},        {0x5c, "dup2"},
        {0x5d, "dup2_x1"},       {0x5e, "dup2_x2"},
        {0x8d, "f2d"},           {0x8b, "f2i"},
        {0x8c, "f2l"},           {0x62, "fadd"},
        {0x30, "falod"},         {0x51, "fastore"},
        {0x96, "fcmpg"},         {0x95, "fcmpl"},
        {0x0b, "fconst_0"},      {0x0c, "fconst_1"},
        {0x0d, "fconst_2"},      {0x6e, "fdiv"},
        {0x17, "fload"},         {0x22, "fload_0"},
        {0x23, "fload_1"},       {0x24, "fload_2"},
        {0x25, "fload_3"},       {0x43, "fstore_0"},
        {0x44, "fstore_1"},      {0x45, "fstore_2"},
        {0x46, "fstore_3"},      {0x66, "fsub"},
        {0xb4, "getfield"},      {0xb2, "getstatic"},
        {0xa7, "goto"},          {0xc8, "goto_w"},
        {0x91, "i2b"},           {0x92, "i2c"},
        {0x87, "i2d"},           {0x86, "i2f"},
        {0x85, "i2l"},           {0x93, "i2s"},
        {0x60, "iadd"},          {0x2e, "iaload"},
        {0x7e, "iand"},          {0x4f, "iastore"},
        {0x02, "iconst_m1"},     {0x03, "iconst_0"},
        {0x04, "iconst_1"},      {0x05, "iconst_2"},
        {0x06, "iconst_3"},      {0x07, "iconst_4"},
        {0x08, "iconst_5"},      {0x6c, "idiv"},
        {0xa5, "if_acmpeg"},     {0xa6, "if_acmpne"},
        {0x9f, "if_icmpeq"},     {0xa2, "if_icmpge"},
        {0xa3, "if_icmpgt"},     {0xa4, "if_icmple"},
        {0xa1, "if_icmplt"},     {0xa0, "if_icmpne"},
        {0x99, "ifeq"},          {0x9c, "ifge"},
        {0x9d, "ifgt"},          {0x9e, "ifle"},
        {0x9b, "iflt"},          {0x9a, "ifne"},
        {0xc7, "ifnonnull"},     {0xc6, "ifnull"},
        {0x84, "iinc"},          {0x15, "iload"},
        {0x1a, "iload_0"},       {0x1b, "iload_1"},
        {0x1b, "iload_1"},       {0x1c, "iload_2"},
        {0x1d, "iload_3"},       {0xfe, "impdep1"},
        {0xff, "impdep2"},       {0xfe, "impdep1"},
        {0xff, "impdep2"},       {0x68, "imul"},
        {0x74, "ineg"},          {0xc1, "instanceof"},
        {0xba, "invokedynamic"}, {0xb9, "invokeinterface"},
        {0xb7, "invokespecial"}, {0xb6, "invokevirtual"},
        {0x80, "ior"},           {0x70, "irem"},
        {0x70, "ireturn"},       {0x7a, "ishl"},
        {0x7a, "ishr"},          {0x36, "istore"},
        {0x3b, "istore_0"},      {0x3c, "istore_1"},
        {0x3e, "istore_2"},      {0x64, "isub"},
        {0x7c, "iushr"},         {0x82, "ixor"},
        {0xa8, "jsr"},           {0xc9, "jsr_w"},
        {0x8a, "l2d"},           {0x89, "l2f"},
        {0x88, "l2i"},           {0x61, "ladd"},
        {0x2f, "laload"},        {0x7f, "land"},
        {0x50, "lastore"},       {0x94, "lcmp"},
        {0x09, "lconst_0"},      {0x0a, "lconst_1"},
        {0x12, "ldc"},           {0x13, "ldc_w"},
        {0x14, "ldc2_2"},        {0x6d, "ldiv"},
        {0x16, "lload"},         {0x1e, "lload_0"},
        {0x1f, "lload_1"},       {0x20, "lload_2"},
        {0x21, "lload_3"},       {0x69, "lmul"},
        {0x75, "lneg"},          {0xab, "lookupswitch"},
        {0x81, "lor"},           {0x71, "lrem"},
        {0xad, "lreturn"},       {0x79, "lshl"},
        {0x7b, "ishr"},          {0x37, "lstore"},
        {0x3f, "lstore_0"},      {0x40, "lstore_1"},
        {0x41, "lstore_2"},      {0x42, "lstore_3"},
        {0x65, "lsub"},          {0x7d, "lushr"},
        {0x83, "lxor"},          {0xc2, "monitorrenter"},
        {0xc3, "monitorrexit"},  {0xc5, "multianewarray"},
        {0xbb, "new"},           {0xbc, "newarray"},
        {0x00, "nop"},           {0x57, "pop"},
        {0x58, "pop2"},          {0xb5, "putfield"},
        {0xb3, "putstatic"},     {0xa9, "ret"},
        {0xb1, "return"},        {0x35, "saload"},
        {0x56, "sastore"},       {0x11, "sipush"},
        {0x5f, "swap"},          {0xaa, "tableswitch"},
        {0xc4, "wide"},
    };
}

///
/// Parses the information of the methods.
///
void MethodInfo::seek() {
    method_count = getInfo(file, 2);

    for (int i = 0; i < method_count; i++) {
        auto access_flags     = getInfo(file, 2);
        auto name_index       = getInfo(file, 2);
        auto descriptor_index = getInfo(file, 2);
        auto attributes_count = getInfo(file, 2);
        std::vector<AttributeCode> ac;
        std::vector<AttributeInfo> ai;
        for (int j = 0; j < attributes_count; j++) {
            auto attr_name_index =
                static_cast<unsigned short int>(getInfo(file, 2));
            if (attr_name_index == code_index) {
                auto attribute_code = readAttrCode(attr_name_index);
                ac.push_back(attribute_code);
            } else {
                auto attribute_info = readAttrInfo(attr_name_index);
                ai.push_back(attribute_info);
            }
        }
        auto file_info =
            MethodInfoCte(access_flags, name_index, descriptor_index,
                          attributes_count, ac, ai);
        mi.push_back(file_info);
    }
}

std::string MethodInfo::getCodeStr(AttributeCode attr) {
    std::stringstream ss;
    for (int j = 0; j < attr.code_length; j++) {
        auto byte_code = static_cast<int>(attr.code[j]);
        if (byte_code > 0) {
            ss << "       " << j;
            for (int i = std::to_string(j).size(); i < 5; i++)
                ss << " ";
            ss << instruction_list[byte_code];
            switch (byte_code) {
            case 0xaa: {
                int instruction_line = j;
                while (j % 4 != 0) { // removes padding bytes
                    j++;
                }
                auto byte1 = static_cast<int>(attr.code[j]);
                j++;
                auto byte2 = static_cast<int>(attr.code[j]);
                j++;
                auto byte3 = static_cast<int>(attr.code[j]);
                j++;
                auto byte4 = static_cast<int>(attr.code[j]);
                j++;
                auto default_ = static_cast<signed int>(
                    (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4);

                auto bytee1 = static_cast<unsigned int>(attr.code[j]);
                j++;
                auto bytee2 = static_cast<unsigned int>(attr.code[j]);
                j++;
                auto bytee3 = static_cast<unsigned int>(attr.code[j]);
                j++;
                auto bytee4 = static_cast<signed int>(attr.code[j]);
                j++;
                auto indeex = static_cast<signed int>(
                    (bytee1 << 24) | (bytee2 << 16) | (bytee3 << 8) | bytee4);

                ss << " 1 to " << indeex << std::endl;

                int k = 1;
                while (k <= indeex) {
                    auto highbyte1 = static_cast<unsigned int>(attr.code[j]);
                    j++;
                    auto highbyte2 = static_cast<unsigned int>(attr.code[j]);
                    j++;
                    auto highbyte3 = static_cast<unsigned int>(attr.code[j]);
                    j++;
                    auto highbyte4 = static_cast<signed int>(attr.code[j]);
                    j++;
                    auto indeeex = static_cast<signed int>(
                        (highbyte1 << 24) | (highbyte2 << 16) |
                        (highbyte3 << 8) | highbyte4);
                    ss << "             " << k << ": "
                       << instruction_line + indeeex << " (+" << indeeex << ")"
                       << std::endl;
                    k++;
                }
                ss << "             "
                   << "default: " << instruction_line + default_ << " (+"
                   << default_ << ")" << std::endl;
                break;
            }
            case 0x17:
            case 0x38:
            case 0x12:
            case 0x16:
            case 0x37:
            case 0xa9:
                ss << " #" << static_cast<unsigned int>(attr.code[j + 1])
                   << "  <"
                   << cp->getNameByIndex(
                          static_cast<unsigned int>(attr.code[j + 1]))
                   << ">" << std::endl;
                j++;
                break;
            case 0x10:
            case 0x15:
            case 0x19:
                ss << " " << static_cast<int>(attr.code[j + 1]) << std::endl;
                j++;
                break;
            case 0x3a:
            case 0xbc:
                ss << " " << static_cast<unsigned int>(attr.code[j + 1])
                   << std::endl;
                j++;
                break;
            case 0x39:
            case 0x36:
            case 0x18:
                ss << "  " << static_cast<unsigned int>(attr.code[++j])
                   << std::endl;
                break;
            case 0xbd:
            case 0xc0:
            case 0xb4:
            case 0xb2:
            case 0xc1:
            case 0xb7:
            case 0xb8:
            case 0xb6:
            case 0xa8:
            case 0x13:
            case 0x14:
            case 0xbb:
            case 0xb5:
            case 0xb3:
                ss << "  #"
                   << (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2])
                   << " "
                   << cp->getNameByIndex(
                          (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2]))
                   << std::endl;
                j += 2;
                break;
            case 0x11: {
                union int_byte {
                    unsigned char buf[2];
                    short int number;
                } integer;
                integer.buf[0] = attr.code[j + 2];
                integer.buf[1] = attr.code[j + 1];
                ss << " " << integer.number << std::endl;
                j += 2;
            } break;
            case 0x84:
                ss << "  #" << static_cast<int>(attr.code[j + 1]) << " "
                   << "by " << static_cast<int>(attr.code[j + 2]) << std::endl;
                j += 2;
                break;
            case 0xc5:
                ss << (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2])
                   << " "
                   << cp->getNameByIndex(
                          (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2]))
                   << std::endl;
                j += 3;
                break;
            case 0x9f:
            case 0xa2:
            case 0xa3:
            case 0xa4:
            case 0xa1:
            case 0x99:
            case 0x9c:
            case 0x9d:
            case 0x9e:
            case 0x9b:
            case 0x9a:
            case 0xc7:
            case 0xc6:
            case 0xa7:
            case 0xa5:
            case 0xa6:
                ss << "  #"
                   << (j +
                       static_cast<short int>(
                           (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                           static_cast<unsigned int>(attr.code[j + 2])))
                   << "  +("
                   << static_cast<short int>(
                          (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2]))
                   << ") " << std::endl;
                j += 2;
                break;
            case 0xb9:
            case 0xba:
                ss << "  #"
                   << (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2])
                   << " "
                   << cp->getNameByIndex(
                          (static_cast<unsigned int>(attr.code[j + 1]) << 8) +
                          static_cast<unsigned int>(attr.code[j + 2]))
                   << std::endl;
                j += 4;
                break;
            case 0xc9:
            case 0xc8:
                ss << "  #"
                   << (static_cast<unsigned int>(attr.code[j + 1]) << 24) +
                          static_cast<unsigned int>(attr.code[j + 2] << 16) +
                          static_cast<unsigned int>(attr.code[j + 3] << 8) +
                          static_cast<unsigned int>(attr.code[j + 4])
                   << " "
                   << cp->getNameByIndex(
                          (static_cast<unsigned int>(attr.code[j + 1]) << 24) +
                          static_cast<unsigned int>(attr.code[j + 2] << 16) +
                          static_cast<unsigned int>(attr.code[j + 3] << 8) +
                          static_cast<unsigned int>(attr.code[j + 4]))
                   << std::endl;
                j += 4;
                break;
            default:
                ss << std::endl;
                break;
            }
        }
    }
    return ss.str();
}

AttributeInfo MethodInfo::readAttrInfo(short unsigned int attr_name_index) {
    auto attr_length = getInfo(file, 4);
    auto attribute_info =
        reinterpret_cast<unsigned char *>(getInfoRaw(file, attr_length).data());
    AttributeInfo ai{attr_name_index, attr_length, attribute_info};
    return ai;
}

AttributeCode MethodInfo::readAttrCode(short unsigned int attr_name_index) {

    auto attr_length = static_cast<unsigned int>(getInfo(file, 4));
    auto max_stack   = static_cast<unsigned short int>(getInfo(file, 2));
    auto max_locals  = static_cast<unsigned short int>(getInfo(file, 2));
    auto code_length = static_cast<unsigned int>(getInfo(file, 4));
    auto code        = getInfoRaw(file, code_length);
    auto et_length   = static_cast<unsigned short int>(getInfo(file, 2));
    exception *et    = new exception[et_length];

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
            unsigned int attribute_length               = getInfo(file, 4);
            unsigned short int line_number_table_length = getInfo(file, 2);
            auto linetable                              = LineTableNumber{
                static_cast<unsigned short int>(name_index),
                attribute_length,
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
            auto attr_len = getInfo(file, 4);
            auto info     = reinterpret_cast<unsigned char *>(
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
        attr_name_index,
        attr_length,
        max_stack,
        max_locals,
        code_length,
        code,
        et_length,
        et,
        attributes_count,
        ai,
        hasLT,
        ltn,
        "",
    };
    return ac;
}

std::vector<MethodInfoCte> *MethodInfo::getMethodInfo() { return &mi; }

void MethodInfo::show() {
    std::cout << "MethodInfo" << std::endl;
    std::cout << "-------------------" << std::endl;
    for (auto elem : mi) {
        std::cout << "method: " << elem.name << "\nDesc:" << elem.descriptor
                  << std::endl;
        std::cout << "access flag [" << access_flags[elem.access_flags]
                  << "] 0x" << std::hex << elem.access_flags << std::dec
                  << std::endl;
        std::cout << "args length " << elem.arg_length << std::endl;
        for (auto attr : elem.attributes) {
            std::cout << "  Attribute: " << attr.name << std::endl
                      << "    info: \n"
                      << "      index: " << attr.attribute_name_index
                      << std::endl
                      << "      length: " << attr.attribute_length << std::endl;
            auto code_length = attr.code_length;
            if (code_length) {
                std::cout << "      code: " << std::endl;
                std::cout << getCodeStr(attr);
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
                std::cout << "          Attribute length: "
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
        }
        // for (auto elem : elem.attributes_info) {
        //     std::cout << "Atribute Name " << elem.name << std::endl;
        //     std::cout << "Attribute Length" << elem.attribute_length
        //               << std::endl;
        // }
    }
    std::cout << std::endl;
}

int MethodInfo::miCount() { return method_count; }

int MethodInfo::getMethodArgsLength(std::string methodName) {
    for (auto method : mi) {
        if (method.name == methodName) {
            return method.arg_length;
        }
    }
    return -1;
}

MethodInfoCte MethodInfo::getMainMethod() {
    MethodInfoCte *mir = nullptr;
    for (auto &method : mi) {
        if (method.name == "main") {
            mir = &method;
            break;
        }
    }
    if (mir == nullptr) {
        throw std::runtime_error(
            "Missing main method, no entry point, aborting");
    }
    return *mir;
}