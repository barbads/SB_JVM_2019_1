#ifndef _FileReader_H_
#define _FileReader_H_

#include <fstream>
#include <vector>

class FileReader {
  protected:
    int getTag(std::ifstream *file);
    unsigned int getInfo(std::ifstream *file, int offset);
    std::string getUTF8Data(std::ifstream *file, int lenght);
    std::vector<unsigned char> getInfoRaw(std::ifstream *file, int offset);
    std::string getInfoHex(std::ifstream *file, int offset);
};

#endif