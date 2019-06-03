#include <ClassFile.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char const *argv[]) {
    ifstream file;
    if (argc < 2 || argc > 2) {
        cout << "You must pass ONE file as argument!" << endl;
        return 0;
    } else if (argc == 2) {
        file = ifstream(argv[1], ios::binary);
    }
    auto cf = ClassFile(&file);
    cf.parse();
    return 0;
}
