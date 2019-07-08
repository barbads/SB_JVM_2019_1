#ifndef _ConstantPool_H_
#define _ConstantPool_H_

#include <DotClassReader/FileReader.hpp>
#include <JVM/structures/DoubleLong.hpp>
#include <JVM/structures/IntFloatReference.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

/**
 * ConstantPool is a class responsible for read the constant pool section of a
 * .class file and save its structures into a vector of pairs containing entry
 * tag and void pointer. The entry tag identifies the void pointer, this allows
 * us to cast/decast from/to void* to desired constant structure
 */
class ConstantPool : FileReader {
  private:
    int pool_size;
    std::ifstream *file;
    std::vector<std::pair<int, std::shared_ptr<void>>> constant_pool;
    void add_to_pool(int tag);
    std::string resolve(int idx);
    void resolve_pool();

  public:
    ConstantPool(std::ifstream *file);
    void seek();
    void show();
    std::string getNameByIndex(int index);
    int getLineTableIndex();
    int getCodeIndex();
    int getMethodNameIndex(int index);
    int cpCount();
    std::string getMethodNameByIndex(int index);
    DoubleLong getNumberByIndex(int index);
    IntFloatReference getValueByIndex(int index);
    std::string getNameAndTypeByIndex(int index);
    std::string getFieldByIndex(int index);
    std::string getClassNameFromMethodByIndex(int index);
    std::vector<std::string> getExternalClasses(std::string this_class);
    int getMethodIndexByName(std::string);
};

#endif