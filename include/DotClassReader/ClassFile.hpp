#ifndef _ClassFile_H_
#define _ClassFile_H_
#include <DotClassReader/Attributes.hpp>
#include <DotClassReader/ConstantPool.hpp>
#include <DotClassReader/FieldInfo.hpp>
#include <DotClassReader/Interface.hpp>
#include <DotClassReader/MethodInfo.hpp>
#include <constants/MethodInfoCte.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

class ClassFile : FileReader {
  private:
    int minor, major;
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
    int parseDescriptor(std::string desc);
  public:
    ClassFile(std::ifstream *file);
    void Parse();
    void Show();
    ConstantPool *getCP();
    MethodInfoCte getMainMethod();
};

#endif