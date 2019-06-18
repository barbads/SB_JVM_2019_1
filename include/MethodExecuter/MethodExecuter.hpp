#ifndef _MethodExecuter_H_
#define _MethodExecuter_H_

#include <DotClassReader/ConstantPool.hpp>
#include <JVM/structures/ContextEntry.hpp>
#include <JVM/structures/StackFrame.hpp>

#include <stack>
#include <string>
#include <vector>

class MethodExecuter {
  private:
    std::vector<unsigned char> bytecode;
    bool isInvokeSpecialInstruction(unsigned char byte);
    bool isNewInstruction(unsigned char byte);
    bool isDupInstruction(unsigned char byte);
    ConstantPool *cp;
    // Pair (classname, value)
    StackFrame *sf;
    std::stack<std::pair<std::string, int>> local_operand_stack;

  public:
    MethodExecuter(std::vector<unsigned char> code, ConstantPool *cp);
    void Exec(std::vector<ContextEntry> ce);
};

#endif