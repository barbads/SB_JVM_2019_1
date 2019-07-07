#ifndef _ATTRIBUTEINFO_H_
#define _ATTRIBUTEINFO_H_
#include <string> 

struct AttributeInfo {
    unsigned short int attribute_name_index;
    unsigned int attribute_lenght;
    unsigned char *info;
    std::string name;
};

#endif