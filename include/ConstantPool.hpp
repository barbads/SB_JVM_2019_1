#ifndef _ConstantPool_H_
#define _ConstantPool_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class ConstantPool {
  private:
    int const_pool_count;

  public:
    ConstantPool(std::ifstream *file);
};

#endif