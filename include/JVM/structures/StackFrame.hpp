#ifndef _StackFrame_H_
#define _StackFrame_H_

#include <JVM/structures/ContextEntry.hpp>
#include <stack>
#include <string>

struct StackFrame {
    // lva (local variable array)
    // in both cases bellow first pair element is value type and second is its
    // byte value
    std::vector<ContextEntry *> lva;
    std::stack<ContextEntry *> operand_stack;
    StackFrame(std::vector<ContextEntry *> localVariableArray) {
        lva = localVariableArray;
    }
};

#endif
