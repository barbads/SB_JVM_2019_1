#ifndef _Attributes_H_
#define _Attributes_H_

#include <DotClassReader/FileReader.hpp>
#include <constants/AttributeClassFile.hpp>
#include <iostream>

/**
 * Attributes implements FileReader interface and is the class
 * responsible for read class attributtes section from file.
 * They can be used to correct interpretation of the .class file
 * or be useful for optional tools.
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