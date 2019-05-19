#ifndef _AttributeClassFile_H_
#define _AttributeClassFile_H_
#include <string>
struct AttributeClassFile {
    unsigned short int attribute_name_index;
    unsigned int attribute_length;
    unsigned short int sourcefile_index;
    std::string name;
    std::string sourcefile;
};

#endif