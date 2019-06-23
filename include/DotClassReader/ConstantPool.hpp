#ifndef _ConstantPool_H_
#define _ConstantPool_H_

#include <DotClassReader/FileReader.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

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
};

#endif