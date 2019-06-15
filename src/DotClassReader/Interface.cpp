#include <DotClassReader/Interface.hpp>
#include <iostream>

Interface::Interface(std::ifstream *file) { this->file = file; }

void Interface::show() {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "               Interfaces" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    for (int i = 0; i < interface_count; i++) {
        std::cout << "  Class name: cp_info #" << itf[i] << " " << itf_name[i]
                  << std::endl;
    }
    std::cout << std::endl;
}

std::vector<int> Interface::getITF() { return itf; }

int Interface::itfCount() { return interface_count; }

void Interface::seek() {
    interface_count = getInfo(file, 2);
    for (int i = 0; i < interface_count; i++) {
        itf.push_back(getInfo(file, 2));
    }
}

void Interface::setITF(std::string itf_name) {
    this->itf_name.push_back(itf_name);
}