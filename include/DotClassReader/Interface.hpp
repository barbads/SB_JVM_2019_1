#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <DotClassReader/FileReader.hpp>

/** Represents an interface that is a direct superinterface of
 * the class represented by the .class file or interface type,
 * in the left-to-right order given in the source for the type.
 */
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
    int itfCount();
};

#endif