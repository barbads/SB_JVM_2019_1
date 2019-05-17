#ifndef _METHODINFO_H_
#define _METHODINFO_H_
#include <FileReader.hpp>
#include <constants/MethodInfoCte.hpp>
#include <fstream>
#include <iostream>
#include <vector>

class MethodInfo : FileReader {
  private:
    int method_count;
    std::vector<MethodInfoCte> mi;
    std::ifstream *file;

  public:
    MethodInfo(std::ifstream *file);
    void seek();
    std::vector<MethodInfoCte> *getMethodInfo();
    void showMI();
};

#endif