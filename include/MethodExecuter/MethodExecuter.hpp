#ifndef _MethodExecuter_H_
#define _MethodExecuter_H_

#include <DotClassReader/ConstantPool.hpp>
#include <JVM/structures/ContextEntry.hpp>
#include <JVM/structures/FieldMap.hpp>
#include <JVM/structures/StackFrame.hpp>

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <vector>

/**
 * The MethodExecute class is responsible for receiving all of the relevant
 * .class file information regarding the execution of the bytecode (like
 * ConstantPool, StackFrame, ClassMethods, ClassFields ...).
 */
class MethodExecuter {
  private:
    std::vector<unsigned char> bytecode;
    bool isInvokeSpecialInstruction(unsigned char byte);
    bool isNewInstruction(unsigned char byte);
    bool isDupInstruction(unsigned char byte);
    ConstantPool *cp;
    // Pair (classname, value)
    StackFrame *sf;
    ClassMethods *cm;
    ClassFields *cf;
    std::stack<std::pair<std::string, int>> local_operand_stack;
    unsigned int countArgs(std::string);
    std::function<int(std::string)> getArgsLen;

  public:
    MethodExecuter(ConstantPool *cp, ClassMethods *cm, ClassFields *cf,
                   std::function<int(std::string)> getArgsLen);
    std::shared_ptr<ContextEntry>
    Exec(std::vector<unsigned char> bytecode,
         std::vector<std::shared_ptr<ContextEntry>> *ce);
};

#endif