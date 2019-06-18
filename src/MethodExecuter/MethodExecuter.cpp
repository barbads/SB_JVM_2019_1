#include <MethodExecuter/MethodExecuter.hpp>

MethodExecuter::MethodExecuter(std::vector<unsigned char> code,
                               ConstantPool *cp) {
    bytecode = code;
    this->cp = cp;
}

void MethodExecuter::Exec(std::vector<ContextEntry> ce) {
    sf = new StackFrame(ce);
    std::vector<int> args;
    int args_counter = 0;
    for (auto byte = bytecode.begin(); byte != bytecode.end(); byte++) {
        switch (*byte) {
        case 0x32: // aaload
        {
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top().isArray) {
                auto arrayRef = sf->operand_stack.top().arrayRef;
                sf->operand_stack.pop();
                if (index >= arrayRef.size()) {
                    throw std::runtime_error("ArrayIndexOutOfBoundsException");
                }
                sf->operand_stack.push(arrayRef.at(index));
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x53: // aastore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top().isArray) {
                sf->operand_stack.top().AddToArray(index, value);
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0xbb: // new
        {
            if (byte + 2 >= bytecode.end()) {
                throw std::runtime_error(
                    "New instruction missing missing parameters, code ends "
                    "before two next args");
            }
            int classNameIndex = (static_cast<int>(*(byte + 1)) << 8) +
                                 static_cast<int>(*(byte + 2));
            std::string className = cp->getNameByIndex(classNameIndex);
            auto cevoid           = reinterpret_cast<void *>(&sf->lva);
            auto entry            = ContextEntry(className, L, cevoid);
            sf->operand_stack.push(entry);
            byte += 1;
        } break;
        case 0x59: // dup
        {
            auto top = sf->operand_stack.top();
            sf->operand_stack.push(top);
        } break;
        default:
            break;
        }
    }
    std::cout << "operand stack" << std::endl;
    while (not sf->operand_stack.empty()) {
        std::cout << sf->operand_stack.top().class_name << " ";
        sf->operand_stack.top().PrintValue();
        std::cout << std::endl;
        sf->operand_stack.pop();
    }
}
