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
    string option;
    option = "";
    if (argc < 2 || argc > 3) {
        cout << "You must pass ONE file as argument!" << endl;
        return 0;
    } else if (argc == 2) {
        file = ifstream(argv[1], ios::binary);
    } else if (argc == 3) {
        file   = ifstream(argv[1], ios::binary);
        option = argv[2];
    }

    auto cf = ClassFile(&file, argv[1]);
    cf.seek();
    if (option == "-i") {
        auto jvm = JVM(&cf);
        jvm.Run();
    } else if (option == "-l") {
        cf.show();
    } else if (option == "") {
        cf.show();

        auto jvm = JVM(&cf);
        jvm.Run();
    } else {
        cout << "You must pass -l (leitor) or -i (interpretador) as options!"
             << endl;
        return 0;
    }

    return 0;
}
