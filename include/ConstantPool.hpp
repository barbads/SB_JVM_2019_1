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

  public:
    ConstantPool(std::ifstream *file);
    void seek();
    void showPool();
};

#endif