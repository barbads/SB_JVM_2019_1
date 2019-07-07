#ifndef _FIELDINFO_H_
#define _FIELDINFO_H_
#include <DotClassReader/FileReader.hpp>
#include <constants/FieldInfoCte.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

/**
 * Gives a complete description of the fields declared by
 * the class or interface represented on this .class file
 */

class FieldInfo : FileReader {
  private:
    int field_count;
    std::vector<FieldInfoCte> fi;
    std::ifstream *file;

  public:
    FieldInfo(std::ifstream *file);
    void seek();
    std::vector<FieldInfoCte> *getFieldInfo();
    void show();
    int fiCount();
};

#endif