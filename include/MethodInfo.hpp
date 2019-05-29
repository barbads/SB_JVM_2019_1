#ifndef _METHODINFO_H_
#define _METHODINFO_H_
#include <ConstantPool.hpp>
#include <FileReader.hpp>
#include <constants/AttributeCode.hpp>
#include <constants/MethodInfoCte.hpp>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class MethodInfo : FileReader {
  private:
    int method_count;
    std::vector<MethodInfoCte> mi;
    std::ifstream *file;
    AttributeCode readAttrCode(short unsigned int attr_name_index);
    AttributeInfo readAttrInfo(short unsigned int attr_name_index);
    int line_table_index;
    int code_index;
    std::map<int, std::string> access_flags;
    std::map<int, std::string> instruction_list;
    std::string getCodeStr(AttributeCode attr);
    ConstantPool *cp;

  public:
    MethodInfo(std::ifstream *file, ConstantPool *cp);
    void seek();
    std::vector<MethodInfoCte> *getMethodInfo();
    void showMI();
    int miCount();
};

#endif