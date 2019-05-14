#include <ClassFile.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char const *argv[]) {
    auto file = ifstream("double_aritmetica.class", ios::binary);
    auto cf   = ClassFile(&file);
    cf.parse();
    return 0;
}
