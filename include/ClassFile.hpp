#ifndef _ClassFile_H_
#define _ClassFile_H_
#include <Attributes.hpp>
#include <ConstantPool.hpp>
#include <FieldInfo.hpp>
#include <Interface.hpp>
#include <MethodInfo.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

class ClassFile : FileReader {
  private:
    std::ifstream *file;
    std::string magic;
    std::string version;
    ConstantPool *cp;
    int access_flags;
    int this_class;
    int super_class;
    Interface *itf;
    FieldInfo *fi;
    MethodInfo *mi;
    Attributes *attr;
    std::string getMagicNumber();
    std::map<int, std::string> access_flag;

  public:
    ClassFile(std::ifstream *file);
    void parse();
};

#endif