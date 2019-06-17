#ifndef _JVM_H_
#define _JVM_H_

#include <DotClassReader/ClassFile.hpp>
#include <JVM/structures/StackFrame.hpp>
#include <MethodExecuter/MethodExecuter.hpp>

#include <stack>
#include <vector>

class JVM {
  private:
    std::stack<StackFrame> stack_per_thread;
    ClassFile *class_loader;

  public:
    JVM(ClassFile *cl);
    void Run();
    void executeByteCode(std::vector<unsigned char> code);
};

#endif