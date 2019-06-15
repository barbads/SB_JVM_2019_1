#ifndef _JVM_H_
#define _JVM_H_

#include <JVM/structures/StackFrame.hpp>
#include <stack>

class JVM {
  private:
    std::stack<StackFrame> stack_per_thread;

  public:
    JVM(/* args */);
};

JVM::JVM(/* args */) { stack_per_thread.push(StackFrame('a')); }

#endif