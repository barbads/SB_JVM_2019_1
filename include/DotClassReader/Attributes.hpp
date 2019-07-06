#ifndef _Attributes_H_
#define _Attributes_H_

#include <DotClassReader/FileReader.hpp>
#include <constants/AttributeClassFile.hpp>
#include <iostream>

/**
 * Attributes is the class responsible for read class attributtes section from
 * file and implements FileReader interface
 */
class Attributes : FileReader {
  private:
    int attributes_count;
    std::ifstream *file;
    std::vector<AttributeClassFile> attr;

  public:
    Attributes(std::ifstream *file);
    void seek();
    std::vector<AttributeClassFile> *getClassAttributes();
    void show();
    int attrCount();
};

#endif