#ifndef _StackFrame_H_
#define _StackFrame_H_

#include <stack>
#include <string>

struct StackFrame {
    // lva (local variable array)
    // in both cases bellow first pair element is value type and second is its
    // byte value
    std::stack<std::pair<std::string, int>> lva;
    std::stack<std::pair<std::string, int>> operand_stack;
    StackFrame(unsigned char this_value) {
        lva.push(std::make_pair("ref", this_value));
    }
    StackFrame() {}
};

#endif
