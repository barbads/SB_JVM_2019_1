#ifndef _ConstantPool_H_
#define _ConstantPool_H_

#include <FileReader.hpp>
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

  public:
    ConstantPool(std::ifstream *file);
    void resolve_pool();
    void seek();
    void showPool();
    std::string getNameByIndex(int index);
};

#endif