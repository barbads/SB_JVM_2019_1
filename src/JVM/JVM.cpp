#include <JVM/JVM.hpp>
#include <JVM/structures/ContextEntry.hpp>
#include <iostream>

JVM::JVM(ClassFile *cl) {
    class_loader = cl;
    class_name   = class_loader->getClassName();
}

ClassMethods JVM::convertMethodIntoMap(std::vector<MethodInfoCte> mi) {
    ClassMethods cm;
    for (auto m : mi) {
        cm.insert(std::pair<int, MethodInfoCte>(m.name_index, m));
    }

    return cm;
}

ClassFields JVM::convertFieldIntoMap(std::vector<FieldInfoCte> fi) {
    ClassFields cf;
    for (auto f : fi) {
        if (f.descriptor.length() == 1) {
            int zero     = 0;
            auto zeroref = reinterpret_cast<void *>(&zero);
            cf[f.name]   = std::make_shared<ContextEntry>(
                "", TypeMap.at(f.descriptor), zeroref);
        } else {
            int dimension = 0;
            std::string type;
            for (auto desc = f.descriptor.begin(); desc < f.descriptor.end();
                 desc++) {
                if (*desc == '[') {
                    dimension++;
                    continue;
                }
                type = std::string(desc, f.descriptor.end());
                break;
            }
            std::shared_ptr<ContextEntry> array_operator = nullptr;
            if (dimension > 1) {
                array_operator =
                    std::make_shared<ContextEntry>("", TypeMap.at(type), 1);
                std::shared_ptr<ContextEntry> init = array_operator;
                for (auto i = 0; i < dimension - 1; i++) {
                    auto newarray =
                        std::make_shared<ContextEntry>("", TypeMap.at(type), 1);
                    array_operator->addToArray(0, newarray);
                    array_operator = std::move(array_operator->arrayRef[0]);
                }
                break;
                cf[f.name] = init;
            } else {
                cf[f.name] = std::make_shared<ContextEntry>(type, L, nullptr);
            }
        }
    }
    return cf;
}

/**
 * Gets the information needed from the class_loader class variable, extracts
 * the bytecode and calls the executeByteCode method that will interpret the
 * code.
 */
void JVM::Run() {
    MethodInfoCte main = class_loader->getMainMethod();
    auto all_fields    = class_loader->getFields();
    std::map<std::string, ClassFields> field_map;
    for (auto field : all_fields) {
        field_map.insert(
            std::make_pair(field.first, convertFieldIntoMap(*field.second)));
    }
    auto all_methods = class_loader->getMethods();
    std::map<std::string, ClassMethods> method_map;
    for (auto method : all_methods) {
        method_map.insert(
            std::make_pair(method.first, convertMethodIntoMap(*method.second)));
    }
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
    std::shared_ptr<ContextEntry> main_context(
        new ContextEntry(field_map.at(class_name), class_name));
    std::vector<std::shared_ptr<ContextEntry>> context{main_context};
    stack_per_thread.push(StackFrame(&context));

    auto code = method_attribute_code.code;
    executeByteCode(code, &field_map, &method_map);
}

/**
 * Sets up the context for bytecode execution and calls the method Exec from the
 * MethodExecuter class.
 */
void JVM::executeByteCode(std::vector<unsigned char> code,
                          std::map<std::string, ClassFields> *cf,
                          std::map<std::string, ClassMethods> *cm) {
    auto context = &stack_per_thread.top().lva;
    std::function<int(std::string, std::string)> getArgsLength(
        std::bind(&ClassFile::getMethodArgsLength, class_loader,
                  std::placeholders::_1, std::placeholders::_2));
    auto me = new MethodExecuter(class_loader->getCP(), cm, cf, getArgsLength,
                                 class_name);
    me->Exec(code, context);
}
