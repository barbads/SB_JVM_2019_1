#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <FileReader.hpp>

class Interface : FileReader {
  private:
    unsigned short int interface_count;
    std::ifstream *file;
    std::vector<int> itf;
    std::vector<std::string> itf_name;

  public:
    Interface(std::ifstream *file);
    void seek();
    void show();
    std::vector<int> getITF();
    void setITF(std::string);
};

#endif