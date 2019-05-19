#ifndef _METHODINFO_H_
#define _METHODINFO_H_
#include <FileReader.hpp>
#include <constants/AttributeCode.hpp>
#include <constants/MethodInfoCte.hpp>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

class MethodInfo : FileReader {
  private:
    int method_count;
    std::vector<MethodInfoCte> mi;
    std::ifstream *file;
    AttributeCode readAttr();
    int line_table_index;

  public:
    MethodInfo(std::ifstream *file, int linetableindex);
    void seek();
    std::vector<MethodInfoCte> *getMethodInfo();
    void showMI();
};

#endif