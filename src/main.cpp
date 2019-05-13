#include <ClassFile.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

string getInfo(ifstream *file, int offset) {
    stringstream ss;
    char magic[offset];
    file->read(magic, offset);
    for (auto i = 0; i < offset; i++) {
        ss << static_cast<int>(static_cast<unsigned char>(magic[i]));
    }
    return ss.str();
}

int main(int argc, char const *argv[]) {
    auto file = ifstream("double_aritmetica.class", ios::binary);
    auto cf   = ClassFile(&file);
    cf.parse();
    return 0;
}
