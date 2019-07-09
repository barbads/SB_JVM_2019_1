#ifndef _StackFrame_H_
#define _StackFrame_H_

#include <JVM/structures/ContextEntry.hpp>
#include <stack>
#include <string>

struct StackFrame {
    // lva (local variable array)
    // in both cases below first pair element is value type and second is its
    // byte value
    std::vector<std::shared_ptr<ContextEntry>> lva;
    std::stack<std::shared_ptr<ContextEntry>> operand_stack;
    ~StackFrame() {}
    StackFrame(std::vector<std::shared_ptr<ContextEntry>> localVariableArray) {
        lva = std::vector<std::shared_ptr<ContextEntry>>(
            localVariableArray.size());
        lva           = localVariableArray;
        operand_stack = std::stack<std::shared_ptr<ContextEntry>>();
    }
};

#endif
