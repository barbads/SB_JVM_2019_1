#ifndef _FileReader_H_
#define _FileReader_H_

#include <fstream>
#include <vector>

/**
 * FileReader class is used to read the .class file. All classes used to read
 * the .class file (ClassFile, MethodInfo, AttributesInfo...) inherit this
 * class.
 */
class FileReader {
  protected:
    int getTag(std::ifstream *file);
    unsigned int getInfo(std::ifstream *file, int offset);
    std::string getUTF8Data(std::ifstream *file, int lenght);
    std::vector<unsigned char> getInfoRaw(std::ifstream *file, int offset);
    std::string getInfoHex(std::ifstream *file, int offset);

  public:
    virtual void seek() = 0;
    virtual void show() = 0;
};

#endif