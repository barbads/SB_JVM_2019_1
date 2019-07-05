#ifndef _JVM_H_
#define _JVM_H_

#include <DotClassReader/ClassFile.hpp>
#include <JVM/structures/FieldMap.hpp>
#include <JVM/structures/StackFrame.hpp>
#include <MethodExecuter/MethodExecuter.hpp>

#include <functional>
#include <stack>
#include <vector>

class JVM {
  private:
    std::stack<StackFrame> stack_per_thread;
    ClassFile *class_loader;
    ClassFields convertFieldIntoMap(std::vector<FieldInfoCte>);
    ClassMethods convertMethodIntoMap(std::vector<MethodInfoCte>);

  public:
    JVM(ClassFile *cl);
    void Run();
    void executeByteCode(std::vector<unsigned char> code, ClassFields *cf,
                         ClassMethods *cm);
};

#endif