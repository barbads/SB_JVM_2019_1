#ifndef _ConstantPool_H_
#define _ConstantPool_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class ConstantPool {
  private:
    int pool_size;
    std::ifstream *file;
    int getTag(std::ifstream *file);
    std::vector<std::pair<int, std::shared_ptr<void>>> constant_pool;
    void add_to_pool(int tag);
    int getInfo(std::ifstream *file, int offset);
    std::string getUTF8Data(std::ifstream *file, int lenght);
    std::vector<unsigned char> getInfoRaw(std::ifstream *file, int offset);

  public:
    ConstantPool(std::ifstream *file);
    void seek();
    void showPool();
};

#endif