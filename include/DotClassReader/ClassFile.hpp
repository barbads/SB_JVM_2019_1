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

/**
 * ClassFile receives .class file and the file name.
 * This class will have all the necessary informations for
 * showing and executing the Java bytecode.
 * Each .class file represents a class or a Java program interface.
 */
class ClassFile : FileReader {
  private:
    int minor, major;
    std::ifstream *file;
    std::string magic;
    std::string version;
    ConstantPool *cp;
    std::map<std::string, ConstantPool *> cp_map;
    std::map<std::string, MethodInfo *> mi_map;
    std::map<std::string, FieldInfo *> fi_map;
    std::map<std::string, Interface *> itf_map;
    std::map<std::string, Attributes *> attr_map;
    int access_flags;
    int this_class;
    int super_class;
    std::map<std::string, std::string> super_map;
    Interface *itf;
    FieldInfo *fi;
    MethodInfo *mi;
    Attributes *attr;
    std::string getMagicNumber();
    std::map<int, std::string> access_flag;
    std::string fileName;
    int parseDescriptor(std::string desc);
    void createCPMap(std::vector<std::string> external_classes);

  public:
    ClassFile(std::ifstream *file, char const *fileName);
    void seek();
    void show();
    std::map<std::string, ConstantPool *> getCP();
    MethodInfoCte getMainMethod();
    std::map<std::string, std::vector<FieldInfoCte> *> getFields();
    std::map<std::string, std::vector<MethodInfoCte> *> getMethods();
    int getMethodArgsLength(std::string className, std::string methodName);
    std::string getClassName();
    std::map<std::string, std::string> getSuper(std::string class_name);
};

#endif