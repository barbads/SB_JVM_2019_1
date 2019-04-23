#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>  
#include <map>

using namespace std;

string getMagic(ifstream *file, int offset) {
    stringstream ss;
    char magic[offset];
    file->read(magic, offset);
    for (auto i = 0; i < offset; i++) {
        ss << hex << static_cast<int>(static_cast<unsigned char>(magic[i]));
    }
    return ss.str();
}

string getInfo(ifstream *file, int offset) {
    stringstream ss;
    char magic[offset];
    file->read(magic, offset);
    for (auto i = 0; i < offset; i++) {
        ss << static_cast<int>(static_cast<unsigned char>(magic[i]));
    }
    return ss.str();
}


int main(int argc, char const *argv[])
{
    auto file = ifstream("double_aritmetica.class", ios::binary);
    auto magic = getMagic(&file, 4);
    auto minor = getInfo(&file, 2);
    auto major = getInfo(&file, 2);
    auto pool_count = stoi(getInfo(&file, 2));
    cout << magic; 
    cout << " " << major << "." << minor << endl;
    auto tag = getInfo(&file, 1);
    return 0;
}
