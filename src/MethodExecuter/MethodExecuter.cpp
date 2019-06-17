#include <MethodExecuter/MethodExecuter.hpp>

MethodExecuter::MethodExecuter(std::vector<unsigned char> code,
                               ConstantPool *cp) {
    bytecode = code;
    this->cp = cp;
}

void MethodExecuter::Exec() {
    std::vector<int> args;
    int args_counter = 0;
    for (auto byte = bytecode.begin(); byte != bytecode.end(); byte++) {
        auto byte_value = static_cast<int>(*byte);
        if (isNewInstruction(byte_value)) {
            if (byte + 2 >= bytecode.end()) {
                throw std::runtime_error(
                    "New instruction missing missing parameters, code ends "
                    "before two next args");
            }
            int classNameIndex = (static_cast<int>(*(byte + 1)) << 8) +
                                 static_cast<int>(*(byte + 2));
            std::string className = cp->getNameByIndex(classNameIndex);
            auto newClass         = std::make_pair(
                className, reinterpret_cast<std::uintptr_t>(&classNameIndex));
            sf.operand_stack.push(newClass);
            byte += 1;
        }
        if (isDupInstruction(byte_value)) {
            auto top = sf.operand_stack.top();
            sf.operand_stack.push(top);
        }
    }
    std::cout << "operand stack" << std::endl;
    while (not sf.operand_stack.empty()) {
        std::cout << sf.operand_stack.top().first << " "
                  << static_cast<unsigned int>(sf.operand_stack.top().second)
                  << std::endl;
        sf.operand_stack.pop();
    }
}

bool MethodExecuter::isDupInstruction(unsigned char byte) {
    return byte == 0x59;
}

bool MethodExecuter::isInvokeSpecialInstruction(unsigned char byte) {
    return byte == 0xb7;
}

bool MethodExecuter::isNewInstruction(unsigned char byte) {
    return byte == 0xbb;
}