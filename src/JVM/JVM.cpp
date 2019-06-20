#include <JVM/JVM.hpp>
#include <iostream>
#include <JVM/structures/FieldMap.hpp>

JVM::JVM(ClassFile *cl) { class_loader = cl; }

void JVM::Run() {
    MethodInfoCte main = class_loader->getMainMethod();
    auto field_vector = class_loader->getFields();
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
    ContextEntry main_context;
    std::vector<ContextEntry> context{main_context};
    stack_per_thread.push(StackFrame(context));

    auto code = method_attribute_code.code;
    executeByteCode(code);
}

void JVM::executeByteCode(std::vector<unsigned char> code) {
    auto context = stack_per_thread.top().lva;
    auto me      = new MethodExecuter(code, class_loader->getCP());
    me->Exec(context);
}