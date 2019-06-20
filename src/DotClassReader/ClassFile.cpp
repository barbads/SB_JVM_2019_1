
#include <DotClassReader/ClassFile.hpp>

ClassFile::ClassFile(std::ifstream *file) {
    this->file  = file;
    this->cp    = new ConstantPool(this->file);
    access_flag = std::map<int, std::string>{
        {0x01, "acc_public"},  {0x02, "acc_private"},
        {0x21, "public"},      {0x4, "acc_protected"},
        {0x08, "acc_static"},  {0x09, "public static"},
        {0x10, "acc_final"},   {0x400, "acc_abstract"},
        {0x800, "acc_strict"},
    };
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
void ClassFile::Parse() {
    magic = getMagicNumber();
    if (magic != "cafebabe") {
        throw std::range_error("Invalid .class file, "
                               "could not read magic number properly");
    }

    minor   = getInfo(file, 2);
    major   = getInfo(file, 2);
    version = major + "." + minor;

    cp->seek();
    access_flags = getInfo(file, 2);
    this_class   = getInfo(file, 2);
    super_class  = getInfo(file, 2);

    itf = new Interface(file);
    itf->seek();
    auto itf_ref = itf->getITF();
    for (auto interface : itf_ref) {
        auto name = cp->getNameByIndex(interface);
        itf->setITF(name);
    }

    fi = new FieldInfo(file);
    fi->seek();
    auto field_info = fi->getFieldInfo();
    for (auto &field : *field_info) {
        field.name       = cp->getNameByIndex(field.name_index);
        field.descriptor = cp->getNameByIndex(field.descriptor_index);
        for (int i = 0; i < field.attributes_count; i++) {
            field.attributes[i].name =
                cp->getNameByIndex(field.attributes[i].attribute_name_index);
        }
    }

    mi = new MethodInfo(file, cp);
    mi->seek();
    auto method_info = mi->getMethodInfo();
    for (auto &method : *method_info) {
        method.name       = cp->getNameByIndex(method.name_index);
        method.descriptor = cp->getNameByIndex(method.descriptor_index);
        method.arg_length = parseDescriptor(method.descriptor);
        for (auto &elem : method.attributes) {
            elem.name = cp->getNameByIndex(elem.attribute_name_index);
        }
    }

    attr = new Attributes(file);
    attr->seek();
    auto attr_list = attr->getClassAttributes();
    for (auto &attribute : *attr_list) {
        attribute.name = cp->getNameByIndex(attribute.attribute_name_index);
        attribute.sourcefile = cp->getNameByIndex(attribute.sourcefile_index);
    }
}

// We can implement method type args here
int ClassFile::parseDescriptor(std::string desc) {
    auto args_start    = desc.find_first_of("(") + 1;
    auto args_end      = desc.find_first_of(")");
    auto args_to_parse = desc.substr(args_start, args_end - 1);
    int length         = 0;
    if (args_to_parse.size() == 0) {
        return 0;
    }
    for (auto l = desc.begin(); l < desc.end(); l++) {
        switch (*l) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'J':
        case 'S':
        case 'Z':
            length++;
            break;
        case 'L': {
            auto end_arg_pos =
                desc.find_first_of(";", std::distance(desc.begin(), l));
            length++;
            l += end_arg_pos;
        } break;
        default:
            break;
        }
    }
    std::cout << length << std::endl;
    return length;
};

void ClassFile::Show() {

    std::cout << "General Information" << std::endl;
    std::cout << "Minor " << minor << std::endl;
    std::cout << "Major " << major << std::endl;
    std::cout << "Constant Pool Count " << cp->cpCount() << std::endl;
    std::cout << "access flags [" << access_flag[access_flags] << "] 0x"
              << std::hex << access_flags << std::dec << std::endl;
    std::cout << "this_class #" << this_class << " "
              << cp->getNameByIndex(this_class) << std::endl;
    std::cout << "super_class #" << super_class << " "
              << cp->getNameByIndex(super_class) << std::endl;
    std::cout << "Interfaces Count " << itf->itfCount() << std::endl;
    std::cout << "Fields Count " << fi->fiCount() << std::endl;
    std::cout << "Methods Count " << mi->miCount() << std::endl;
    std::cout << "Attributes Count " << attr->attrCount() << std::endl;
    std::cout << "-------------\n" << std::endl;

    cp->show();
    itf->show();
    fi->showFI();
    mi->showMI();
    attr->show();
}

MethodInfoCte ClassFile::getMainMethod() {
    // if none main method is found, throws runtime exception
    return mi->getMainMethod();
}

ConstantPool *ClassFile::getCP() { return cp; }

std::vector<FieldInfoCte> *ClassFile::getFields() { return fi->getFieldInfo(); }