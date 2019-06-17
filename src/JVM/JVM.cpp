#include <JVM/JVM.hpp>
#include <iostream>

JVM::JVM(ClassFile *cl) { class_loader = cl; }

void JVM::Run() {
    MethodInfoCte main = class_loader->getMainMethod();
    if (main.attributes_count < 1) {
        throw std::out_of_range(
            "Method main must have only one code attribute, check .class file");
    }
    auto method_attribute_code = main.attributes[0];
    auto code_length           = method_attribute_code.code_length;
    if (!code_length) {
        std::cout << "No code to be executed" << std::endl;
        return;
    }

    auto code = method_attribute_code.code;
    executeByteCode(code);
}

void JVM::executeByteCode(std::vector<unsigned char> code) {
    auto me = new MethodExecuter(code, class_loader->getCP());
    me->Exec();
}