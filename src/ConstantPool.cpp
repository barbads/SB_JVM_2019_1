#include <ConstantPool.hpp>
#include <constants/Class.hpp>
#include <constants/Double.hpp>
#include <constants/Fieldref.hpp>
#include <constants/Float.hpp>
#include <constants/Integer.hpp>
#include <constants/InterfaceMethodref.hpp>
#include <constants/Long.hpp>
#include <constants/Methodref.hpp>
#include <constants/NameAndType.hpp>
#include <constants/String.hpp>
#include <constants/UTF8.hpp>
#include <cstring>

ConstantPool::ConstantPool(std::ifstream *file) {
    this->file = file;
    file->seekg(0);
    file->seekg(8);
    char constant_pool[2];
    auto ss = std::stringstream();
    file->read(constant_pool, 2);
    for (auto i = 0; i < 2; i++) {
        ss << static_cast<int>(static_cast<unsigned char>(constant_pool[i]));
    }

    pool_size = stoi(ss.str());
}

void ConstantPool::seek() {
    file->seekg(0);
    file->seekg(10);
    for (auto i = 0; i < pool_size - 1; i++) {
        // do `pool_size` iterations over the file
        auto tag = getTag(file);
        // case double or float we need to skip one iteration
        if (tag == 5 || tag == 6)
            ++i;
        add_to_pool(tag);
    }
    showPool();
}

void ConstantPool::add_to_pool(int tag) {
    switch (tag) {
    case 7: {
        // Class_info
        auto name_index = getInfo(file, 2);
        auto class_info = std::make_shared<Class>(name_index);
        constant_pool.push_back(std::make_pair(tag, class_info));
    } break;
    case 9: {
        // Fieldred_info
        auto class_index     = getInfo(file, 2);
        auto name_type_index = getInfo(file, 2);
        auto fieldref_info =
            std::make_shared<Fieldref>(class_index, name_type_index);
        constant_pool.push_back(std::make_pair(tag, fieldref_info));
    } break;
    case 12: {
        auto name_index       = getInfo(file, 2);
        auto descriptor_index = getInfo(file, 2);
        auto name_and_type_info =
            std::make_shared<NameAndType>(name_index, descriptor_index);
        constant_pool.push_back(std::make_pair(tag, name_and_type_info));
    } break;
    case 1: {
        auto lenght    = getInfo(file, 2);
        auto data      = getUTF8Data(file, lenght);
        auto utf8_info = std::make_shared<UTF8>(lenght, data);
        constant_pool.push_back(std::make_pair(tag, utf8_info));
    } break;
    case 10: {
        auto class_name_ref = getInfo(file, 2);
        auto name_type      = getInfo(file, 2);
        auto method_ref =
            std::make_shared<Methodref>(class_name_ref, name_type);
        constant_pool.push_back(std::make_pair(tag, method_ref));
    } break;
    case 11: {
        auto class_index         = getInfo(file, 2);
        auto name_and_type_index = getInfo(file, 2);
        auto interfaceMethodRef  = std::make_shared<InterfaceMethodref>(
            class_index, name_and_type_index);
        constant_pool.push_back(std::make_pair(tag, interfaceMethodRef));
    } break;
    case 8: {
        auto string_index = getInfo(file, 2);
        auto stringinfo   = std::make_shared<String>(string_index);
        constant_pool.push_back(std::make_pair(tag, stringinfo));
    } break;
    case 3: {
        auto value   = getInfo(file, 4);
        auto integer = std::make_shared<Integer>(value);
        constant_pool.push_back(std::make_pair(tag, integer));
    } break;
    case 4: {
        auto value = getInfoRaw(file, 4);
        float fvalue;
        memcpy(&fvalue, &value, sizeof(fvalue));
        auto float_info = std::make_shared<Float>(fvalue);
        constant_pool.push_back(std::make_pair(tag, float_info));
    } break;
    case 5: {
        auto high_bytes = getInfoRaw(file, 4);
        auto low_bytes  = getInfoRaw(file, 4);
        auto long_info  = std::make_shared<Long>(high_bytes, low_bytes);
        constant_pool.push_back(std::make_pair(tag, long_info));
    } break;
    case 6: {
        auto high_bytes  = getInfoRaw(file, 4);
        auto low_bytes   = getInfoRaw(file, 4);
        auto double_info = std::make_shared<Double>(high_bytes, low_bytes);
        constant_pool.push_back(std::make_pair(tag, double_info));
    } break;
    default:
        char msg_error[30];
        sprintf(msg_error, "Tag %d is not in scope", tag);
        throw std::domain_error(msg_error);
        break;
    }
}

void ConstantPool::showPool() {
    for (auto elem : constant_pool) {
        switch (elem.first) {
        case 7: {
            auto class_info = std::static_pointer_cast<Class>(elem.second);
            std::cout << "Class_info:" << std::endl
                      << "Name index: " << class_info->name_index << std::endl
                      << std::endl;
        } break;
        case 9: {
            auto fieldref_info =
                std::static_pointer_cast<Fieldref>(elem.second);
            std::cout << "Fieldref_info:" << std::endl
                      << "Class index: " << fieldref_info->class_index
                      << std::endl
                      << "Name and Type index: "
                      << fieldref_info->name_type_index << std::endl
                      << std::endl;
        } break;
        case 12: {
            auto name_type_info =
                std::static_pointer_cast<NameAndType>(elem.second);
            std::cout << "NameAndType_info:" << std::endl
                      << "Name index: " << name_type_info->name_index
                      << "Descriptor index: "
                      << name_type_info->descriptor_index << std::endl
                      << std::endl;
        } break;
        case 1: {
            auto utf8_info = std::static_pointer_cast<UTF8>(elem.second);
            std::cout << "UTF8_info: " << std::endl
                      << "lenght: " << utf8_info->lenght << std::endl
                      << "Data: " << utf8_info->bytes << std::endl
                      << std::endl;
        } break;
        case 10: {
            auto method_ref = std::static_pointer_cast<Methodref>(elem.second);
            std::cout << "Methodref_info:" << std::endl
                      << "Class Name: " << method_ref->class_index << std::endl
                      << "Name and Type: " << method_ref->name_type_index
                      << std::endl
                      << std::endl;
        } break;
        case 11: {
            auto interface_info =
                std::static_pointer_cast<InterfaceMethodref>(elem.second);
            std::cout << "InterfaceMethodref_info:" << std::endl
                      << "Classe index: " << interface_info->class_index
                      << std::endl
                      << "Name and Type index: "
                      << interface_info->name_type_index << std::endl
                      << std::endl;

        } break;
        case 8: {
            auto string_info = std::static_pointer_cast<String>(elem.second);
            std::cout << "String_info:" << std::endl
                      << "String index:" << string_info->string_index
                      << std::endl
                      << std::endl;
        } break;
        case 3: {
            auto integer_info = std::static_pointer_cast<Integer>(elem.second);
            std::cout << "Integer_info:" << std::endl
                      << "bytes: " << integer_info->value << std::endl
                      << std::endl;
        } break;
        case 4: {
            auto float_info = std::static_pointer_cast<Float>(elem.second);
            std::cout << "Float_info:" << std::endl
                      << "bytes: " << float_info->value << std::endl
                      << std::endl;
        } break;
        case 5: {
            auto long_info = std::static_pointer_cast<Long>(elem.second);
            std::cout << "Long_info:" << std::endl
                      << "high bytes: " << long_info->high_bytes << std::endl
                      << "low bytes: " << long_info->low_bytes << std::endl
                      << std::endl;
        } break;
        case 6: {
            auto double_info = std::static_pointer_cast<Double>(elem.second);
            std::cout << "Double_info" << std::endl
                      << "high bytes: " << double_info->high_bytes << std::endl
                      << "low bytes: " << double_info->low_bytes << std::endl
                      << std::endl;
        } break;
        default:
            char msg_error[30];
            sprintf(msg_error, "Tag %d is not in scope", elem.first);
            throw std::domain_error(msg_error);
            break;
        }
    }
}

int ConstantPool::getTag(std::ifstream *file) {
    std::stringstream ss;
    char tag[1];
    file->read(tag, 1);
    ss << static_cast<int>(static_cast<unsigned char>(tag[0]));
    return stoi(ss.str());
}

int ConstantPool::getInfo(std::ifstream *file, int offset) {
    std::stringstream ss;
    char tag[offset];
    file->read(tag, offset);
    for (auto i = 0; i < offset; ++i) {
        ss << static_cast<int>(static_cast<unsigned char>(tag[i]));
    }
    return stoi(ss.str());
}

std::vector<unsigned char> ConstantPool::getInfoRaw(std::ifstream *file,
                                                    int offset) {
    std::stringstream ss;
    char tag[offset];
    unsigned char retval[offset];
    file->read(tag, offset);
    memcpy(retval, tag, offset);
    std::vector<unsigned char> ucharvec(retval, retval + offset);
    return ucharvec;
}

std::string ConstantPool::getUTF8Data(std::ifstream *file, int lenght) {
    std::stringstream ss;
    char tag[lenght];
    file->read(tag, lenght);
    for (auto i = 0; i < lenght; ++i) {
        ss << (static_cast<unsigned char>(tag[i]));
    }
    return ss.str();
}