#ifndef _METHODINFO_H_
#define _METHODINFO_H_
#include <DotClassReader/ConstantPool.hpp>
#include <DotClassReader/FileReader.hpp>
#include <constants/AttributeCode.hpp>
#include <constants/MethodInfoCte.hpp>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

/**
 * MethodInfo is the class responsible for read class methods section from
 * file and implements FileReader interface
 */
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
    void show();
    int miCount();
    MethodInfoCte getMainMethod();
    int getMethodArgsLength(std::string methodName);
};

#endif