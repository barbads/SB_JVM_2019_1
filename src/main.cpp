#include <ConstantPool.hpp>
#include <MagicNumber.hpp>
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
    auto file           = ifstream("double_aritmetica.class", ios::binary);
    auto magicExtractor = MagicNumber(&file);
    auto constant_pool  = ConstantPool(&file);
    auto magic          = magicExtractor.getMagicNumber();
    auto minor          = getInfo(&file, 2);
    auto major          = getInfo(&file, 2);
    cout << magic;
    cout << " " << major << "." << minor << endl;
    constant_pool.seek();
    return 0;
}
