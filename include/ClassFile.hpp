#ifndef _ClassFile_H_
#define _ClassFile_H_
#include <ConstantPool.hpp>
#include <FieldInfo.hpp>
#include <Interface.hpp>
#include <MethodInfo.hpp>
#include <fstream>
#include <iostream>
#include <string>

class ClassFile : FileReader {
  private:
    std::ifstream *file;
    std::string magic;
    std::string version;
    ConstantPool *cp;
    std::string access_flags;
    int this_class;
    int super_class;
    Interface *itf;
    FieldInfo *fi;
    MethodInfo *mi;
    std::string getMagicNumber();

  public:
    ClassFile(std::ifstream *file);
    void parse();
};

#endif