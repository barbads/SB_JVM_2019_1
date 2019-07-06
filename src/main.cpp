#include <DotClassReader/ClassFile.hpp>
#include <JVM/JVM.hpp>
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
    auto cf = ClassFile(&file, argv[1]);
    cf.seek();
    // If you want to show class info, just uncomment the line bellow
    cf.show();

    auto jvm = JVM(&cf);
    jvm.Run();

    return 0;
}
