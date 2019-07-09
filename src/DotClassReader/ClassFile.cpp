
#include <DotClassReader/ClassFile.hpp>
#include <fstream>

ClassFile::ClassFile(std::ifstream *file, char const *fileName) {
    this->file     = file;
    this->cp       = new ConstantPool(this->file);
    this->fileName = fileName;
    access_flag    = std::map<int, std::string>{
        {0x01, "acc_public"},  {0x02, "acc_private"},
        {0x21, "public"},      {0x4, "acc_protected"},
        {0x08, "acc_static"},  {0x09, "public static"},
        {0x10, "acc_final"},   {0x400, "acc_abstract"},
        {0x800, "acc_strict"},
    };
}

ClassFile::~ClassFile() {
    delete itf;
    delete cp;
    delete fi;
    delete attr;
}
///
/// Gets the first for bytes fo the .class file, also known as Magic Number.
///
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

///
/// Parses the .class file and fills the structures from the ClassFile class.
/// This method used to be called Parse, but due to interface implementation was
/// changed to seek.
///
void ClassFile::seek() {
    magic = getMagicNumber();
    if (magic != "cafebabe") {
        throw std::range_error("Invalid .class file, "
                               "could not read magic number properly");
    }

    minor   = getInfo(file, 2);
    major   = getInfo(file, 2);
    version = std::to_string(major) + "." + std::to_string(minor);

    cp->seek();
    access_flags = getInfo(file, 2);
    this_class   = getInfo(file, 2);
    super_class  = getInfo(file, 2);

    std::string base_filename =
        this->fileName.substr(fileName.find_last_of("/") + 1);
    if ((cp->getNameByIndex(this_class) + ".class") != base_filename) {
        throw std::range_error("Invalid .class file, "
                               "file name is not the same as this class");
    }

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

    auto external_classes =
        cp->getExternalClasses(cp->getNameByIndex(this_class));
    createCPMap(external_classes);
}

///
/// Parses descriptor of functions to return arguments size
///
int ClassFile::parseDescriptor(std::string desc) {
    auto args_start    = desc.find_first_of("(") + 1;
    auto args_end      = desc.find_first_of(")");
    auto args_to_parse = desc.substr(args_start, args_end - 1);
    int length         = 0;
    if (args_to_parse.size() == 0) {
        return 0;
    }
    for (auto l = desc.begin(); *l != ')'; l++) {
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
            l += end_arg_pos - 2;
        } break;
        default:
            break;
        }
    }
    // std::cout << length << std::endl;
    return length;
};

///
/// Shows .class file information.
///
void ClassFile::show() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "       ClassFile General Information" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
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
    fi->show();
    mi->show();
    attr->show();
}

///
/// Checks if "main" method exists.
///
MethodInfoCte ClassFile::getMainMethod() {
    // if none main method is found, throws runtime exception
    return mi->getMainMethod();
}

///
/// Returns ConstantPool object
///
std::map<std::string, ConstantPool *> ClassFile::getCP() { return cp_map; }

///
/// Returns Fields information
///
std::map<std::string, std::vector<FieldInfoCte> *> ClassFile::getFields() {
    std::map<std::string, std::vector<FieldInfoCte> *> field_map;
    for (auto field : fi_map)
        field_map.insert(
            std::make_pair(field.first, field.second->getFieldInfo()));
    return field_map;
}

///
/// Returns Methods information
///
std::map<std::string, std::vector<MethodInfoCte> *> ClassFile::getMethods() {
    std::map<std::string, std::vector<MethodInfoCte> *> method_map;
    for (auto method : mi_map)
        method_map.insert(
            std::make_pair(method.first, method.second->getMethodInfo()));
    return method_map;
}

///
/// Returns argument length of methodName
///
int ClassFile::getMethodArgsLength(std::string className,
                                   std::string methodName) {
    return mi_map.at(className)->getMethodArgsLength(methodName);
}

///
/// Return class_name indicated by this_class index at constant pool
///
std::string ClassFile::getClassName() { return cp->getNameByIndex(this_class); }

void ClassFile::createCPMap(std::vector<std::string> external_classes) {
    for (auto class_name : external_classes) {

        auto classfilename = class_name + ".class";
        std::string directory;
        size_t last_slash_idx = fileName.find_last_of('/');
        if (std::string::npos != last_slash_idx) {
            directory = fileName.substr(0, last_slash_idx);
        } else {
            last_slash_idx = fileName.find_last_of('\\');
            if (std::string::npos != last_slash_idx) {
                directory = fileName.substr(0, last_slash_idx);
            }
        }
        std::ifstream *file;
        if (directory != "")
            file = new std::ifstream(directory + "/" + classfilename,
                                     std::ios::binary);
        else
            file = new std::ifstream(classfilename, std::ios::binary);
        auto magic = getMagicNumber();
        if (magic != "cafebabe") {
            throw std::range_error("Invalid .class file, "
                                   "could not read magic number properly");
        }

        auto minor   = getInfo(file, 2);
        auto major   = getInfo(file, 2);
        auto version = std::to_string(major) + "." + std::to_string(minor);

        auto external_class_cp = new ConstantPool(file);
        external_class_cp->seek();
        auto access_flags = getInfo(file, 2);
        auto this_class   = getInfo(file, 2);
        auto super_class  = getInfo(file, 2);
        super_map.insert(
            std::make_pair(external_class_cp->getNameByIndex(this_class),
                           external_class_cp->getNameByIndex(super_class)));
        cp_map.insert(std::make_pair(class_name, external_class_cp));

        auto itf_external = new Interface(file);
        itf_external->seek();
        auto itf_ref = itf_external->getITF();
        for (auto interface : itf_ref) {
            auto name = external_class_cp->getNameByIndex(interface);
            itf_external->setITF(name);
        }
        itf_map.insert(std::make_pair(class_name, itf_external));

        auto fi_external = new FieldInfo(file);
        fi_external->seek();
        auto field_info = fi_external->getFieldInfo();
        for (auto &field : *field_info) {
            field.name = external_class_cp->getNameByIndex(field.name_index);
            field.descriptor =
                external_class_cp->getNameByIndex(field.descriptor_index);
            for (int i = 0; i < field.attributes_count; i++) {
                field.attributes[i].name = external_class_cp->getNameByIndex(
                    field.attributes[i].attribute_name_index);
            }
        }
        fi_map.insert(std::make_pair(class_name, fi_external));

        auto mi_external = new MethodInfo(file, external_class_cp);
        mi_external->seek();
        auto method_info = mi_external->getMethodInfo();
        for (auto &method : *method_info) {
            method.name = external_class_cp->getNameByIndex(method.name_index);
            method.descriptor =
                external_class_cp->getNameByIndex(method.descriptor_index);
            method.arg_length = parseDescriptor(method.descriptor);
            for (auto &elem : method.attributes) {
                elem.name = external_class_cp->getNameByIndex(
                    elem.attribute_name_index);
            }
        }
        mi_map.insert(std::make_pair(class_name, mi_external));

        auto attr_external = new Attributes(file);
        attr_external->seek();
        auto attr_list = attr_external->getClassAttributes();
        for (auto &attribute : *attr_list) {
            attribute.name = external_class_cp->getNameByIndex(
                attribute.attribute_name_index);
            attribute.sourcefile =
                external_class_cp->getNameByIndex(attribute.sourcefile_index);
        }
        attr_map.insert(std::make_pair(class_name, attr_external));
    }
    auto this_name = cp->getNameByIndex(this_class);
    cp_map.insert(std::make_pair(this_name, cp));
    mi_map.insert(std::make_pair(this_name, mi));
    itf_map.insert(std::make_pair(this_name, itf));
    fi_map.insert(std::make_pair(this_name, fi));
    attr_map.insert(std::make_pair(this_name, attr));
    super_map.insert(std::make_pair(cp->getNameByIndex(this_class),
                                    cp->getNameByIndex(super_class)));
}

std::map<std::string, std::string> ClassFile::getSuper(std::string class_name) {
    return super_map;
}