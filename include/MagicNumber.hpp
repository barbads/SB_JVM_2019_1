#ifndef _MagicNumber_H_
#define _MagicNumber_H_
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

class MagicNumber {
  private:
    std::string magicNumber;

  public:
    MagicNumber(std::ifstream *file);
    std::ifstream *file;
    std::string getMagicNumber();
};

#endif
