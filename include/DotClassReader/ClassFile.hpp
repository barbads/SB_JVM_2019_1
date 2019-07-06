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

///
/// ClassFile receives .class file and the file name.
/// This class will have all the necessary informations for showing and
/// executing the Java bytecode.
///
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
    std::string fileName;
    int parseDescriptor(std::string desc);

  public:
    ClassFile(std::ifstream *file, char const *fileName);
    void seek();
    void show();
    ConstantPool *getCP();
    MethodInfoCte getMainMethod();
    std::vector<FieldInfoCte> *getFields();
    std::vector<MethodInfoCte> *getMethods();
    int getMethodArgsLength(std::string methodName);
};

#endif