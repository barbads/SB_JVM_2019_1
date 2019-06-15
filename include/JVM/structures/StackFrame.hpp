#ifndef _StackFrame_H_
#define _StackFrame_H_

#include <stack>
#include <string>

struct StackFrame {
    // lva (local variable array)
    // in both cases bellow first pair element is value type and second is its
    // byte value
    std::stack<std::pair<std::string, unsigned char>> lva;
    std::stack<std::pair<std::string, unsigned char>> operand_stack;
    int cp_index_ref;
    StackFrame(unsigned char this_value) {
        lva.push(std::make_pair("ref", this_value));
    }
};

#endif
