#ifndef _ATTRIBUTEINFO_H_
#define _ATTRIBUTEINFO_H_

struct AttributeInfo {
    unsigned short int attribute_name_index;
    int attribute_lenght;
    unsigned char *info;
};

#endif