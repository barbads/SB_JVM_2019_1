#include <JVM/JVM.hpp>
#include <JVM/structures/ContextEntry.hpp>
#include <iostream>

JVM::JVM(ClassFile *cl) { class_loader = cl; }

ClassFields JVM::convertFieldIntoMap(std::vector<FieldInfoCte> fi) {
    ClassFields cf;
    for (auto f : fi) {
        if (f.descriptor.length() == 1) {
            int zero     = 0;
            auto zeroref = reinterpret_cast<void *>(&zero);
        }
    }
}

void JVM::Run() {
    MethodInfoCte main = class_loader->getMainMethod();
    auto field_vector  = *class_loader->getFields();
    auto field_map     = convertFieldIntoMap(field_vector);
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