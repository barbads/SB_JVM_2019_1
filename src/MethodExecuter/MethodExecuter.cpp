#include <JVM/structures/FieldMap.hpp>
#include <MethodExecuter/MethodExecuter.hpp>
#include <algorithm>
#include <math.h>

MethodExecuter::MethodExecuter(
    std::map<std::string, ConstantPool *> cp,
    std::map<std::string, ClassMethods> *cm,
    std::map<std::string, ClassFields> *cf,
    std::function<int(std::string, std::string)> getArgsLen,
    std::string class_name) {
    this->cm         = cm;
    this->cf         = cf;
    this->getArgsLen = getArgsLen;
    this->class_name = class_name;
    this->cp         = cp;
}

/**
 * MethodExecuter implements and executes all the instructions of the JVM. It
 * sets up the StackFrame, and instructions context to be used with ease. It
 * returns a ContextEntry type when recursive.
 */
std::shared_ptr<ContextEntry>
MethodExecuter::Exec(std::vector<unsigned char> bytecode,
                     std::vector<std::shared_ptr<ContextEntry>> *ce) {
    auto sf_local = new StackFrame(*ce);
    std::vector<int> args;
    auto cf_val      = *(this->cf);
    int args_counter = 0;
    bool wide        = false;
    int i            = 0;
    for (auto byte = bytecode.begin(); byte < bytecode.end(); byte++) {
        i = byte - bytecode.begin();
        switch (*byte) {
        case 0x32: // aaload
        {
            auto index = sf_local->operand_stack.top()->context_value.i;
            sf_local->operand_stack.pop();
            if (sf_local->operand_stack.top()->isArray) {
                auto arrayRef = sf_local->operand_stack.top()->getArray();
                sf_local->operand_stack.pop();
                if (index >= arrayRef->size()) {
                    throw std::runtime_error("ArrayIndexOutOfBoundsException");
                }
                sf_local->operand_stack.push(arrayRef->at(index));
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x53: // aastore
        {
            auto value = sf_local->operand_stack.top();
            std::shared_ptr<ContextEntry> value_ref(
                std::shared_ptr<ContextEntry>(
                    new ContextEntry(std::move(*value))));
            sf_local->operand_stack.pop();
            auto index = sf_local->operand_stack.top()->context_value.i;
            sf_local->operand_stack.pop();
            if (sf_local->operand_stack.top()->isArray) {
                sf_local->operand_stack.top()->addToArray(index, value_ref);
                sf_local->operand_stack.pop();
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x01: // aconst_null
        {
            sf_local->operand_stack.push(
                std::shared_ptr<ContextEntry>(new ContextEntry()));
        } break;
        case 0x19: // aload
        {
            int index        = -1;
            const int index1 = *(++byte);
            if (wide) {
                wide             = false;
                const int index2 = *(++byte);
                index            = index1 << 8 + index2;
            } else {
                index = index1;
            }
            auto load = sf_local->lva.at(index);
            if (load->isReference() && !load->isReturnAddress()) {
                sf_local->operand_stack.push(load);
            } else {
                throw std::runtime_error("aload of non-reference object or "
                                         "load on a return address");
            }
        } break;
        case 0x2a: // aload_0
        case 0x2b: // aload_1
        case 0x2c: // aload_2
        case 0x2d: // aload_3
        {
            auto index = *byte - 0x2a;
            auto load  = sf_local->lva.at(index);
            if (load->isReference() && !load->isReturnAddress()) {
                sf_local->operand_stack.push(load);
            } else {
                throw std::runtime_error("aload of non-reference object or "
                                         "load on a return address");
            }
        } break;
        case 0xbd: // anewarray
        {
            int count = sf_local->operand_stack.top()->context_value.b;
            sf_local->operand_stack.pop();
            const int index1 = *(++byte);
            const int index2 = *(++byte);
            int index        = index1 << 8 | index2;
            if (count < 0) {
                throw std::runtime_error("NegativeArraySizeException");
            }
            // do we need to save this into a heap? Idk
            sf_local->operand_stack.push(
                std::shared_ptr<ContextEntry>(new ContextEntry(
                    cp.at(class_name)->getNameByIndex(index), L, count)));
        } break;
        case 0xb0: // areturn
        {
            auto retval = sf_local->operand_stack.top();
            while (!sf_local->operand_stack.empty()) {
                sf_local->operand_stack.pop();
            }
            if (!retval->isReference())
                throw std::runtime_error(
                    "areturn cannot return a non-reference value");
            return retval;
        } break;
        case 0xbe: // arraylength
        {
            auto arrRef = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (!arrRef->isReference()) {
                std::runtime_error(
                    "arraylength called over a non-reference object");
            }

            auto length     = arrRef->arrayLength();
            auto length_ptr = std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(length)));
            sf_local->operand_stack.push(length_ptr);
        } break;
        case 0x3a: // astore
        {
            int index        = -1;
            const int index1 = *(byte + 1);
            if (wide) {
                wide             = false;
                const int index2 = *(byte + 2);
                index            = index1 << 8 + index2;
                byte++;
            } else {
                index = index1;
            }
            byte++;
            auto objRef = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (!objRef->isReference() || !objRef->isReturnAddress()) {
                std::runtime_error("astore called over a non-reference or "
                                   "returnAddress object");
            }
            // return address type??
            if (index > sf_local->lva.size()) {
                while (index > sf_local->lva.size())
                    sf_local->lva.push_back(std::make_shared<ContextEntry>());
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(objRef);
            } else {
                sf_local->lva[index] = objRef;
            }
        } break;
        case 0x4b: // astore_0
        case 0x4c: // astore_1
        case 0x4d: // astore_2
        case 0x4e: // astore_3
        {
            unsigned int index = *byte - 0x4b;
            auto objRef        = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (!objRef->isReference() || !objRef->isReturnAddress()) {
                std::runtime_error("astore called over a non-reference or "
                                   "returnAddress object");
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(objRef);
            } else {
                sf_local->lva[index] = objRef;
            }
        } break;
        case 0xbf: // athrow
        {
            // implement this;
        } break;
        case 0xbb: // new
        {
            if (byte + 2 >= bytecode.end()) {
                throw std::runtime_error(
                    "New instruction missing missing parameters, code ends "
                    "before two next args");
            }
            int classNameIndex = (static_cast<int>(*(byte + 1)) << 8) +
                                 static_cast<int>(*(byte + 2));
            std::string className =
                cp.at(class_name)->getNameByIndex(classNameIndex);
            if (className.find("java/", 0) == std::string::npos) {
                auto entry = std::shared_ptr<ContextEntry>(
                    new ContextEntry(cf->at(className), className));
                sf_local->operand_stack.push(entry);
            }
            byte += 2;
        } break;
        case 0x59: // dup
        {
            if (!sf_local->operand_stack.empty()) {
                auto top = sf_local->operand_stack.top();
                sf_local->operand_stack.push(top);
            }
        } break;
        case 0x33: // baload
        case 0x34: // caload
        case 0x31: // daload
        case 0x30: // faload
        case 0x2e: // iaload
        case 0x2f: // laload
        case 0x35: // saload
        {
            auto index = sf_local->operand_stack.top()->context_value.b;
            sf_local->operand_stack.pop();
            auto arrayref = sf_local->operand_stack.top()->getArray();
            sf_local->operand_stack.pop();
            auto arrayrefv = *arrayref;
            if (index >= arrayrefv.size())
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            auto value = arrayref->at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x54: // bastore
        case 0x55: // castore
        case 0x52: // dastore
        case 0x51: // fastore
        case 0x4f: // iastore
        case 0x50: // lastore
        case 0x56: // sastore
        {
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto index = sf_local->operand_stack.top()->context_value.b;
            sf_local->operand_stack.pop();
            auto arrayref = sf_local->operand_stack.top()->getArray();
            sf_local->operand_stack.pop();

            std::shared_ptr<ContextEntry> value_ref(
                std::shared_ptr<ContextEntry>(
                    new ContextEntry(std::move(*value))));
            if (index > arrayref->size())
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            if (index == arrayref->size())
                arrayref->push_back(value_ref);
            else {
                arrayref->operator[](index) = value_ref;
            }
        } break;
        case 0x10: // bipush
        {
            int value = *(++byte);
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", B, reinterpret_cast<void *>(&value))));
        } break;
        case 0xc0: // checkcast
        {
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto objref        = sf_local->operand_stack.top();
            if (!objref->isNull) {
                sf_local->operand_stack.pop();
                if (objref->class_name ==
                    cp.at(class_name)->getNameByIndex(index)) {
                    sf_local->operand_stack.push(objref);
                } else {
                    throw std::runtime_error("ClassCastException");
                }
            }
        } break;
        case 0x90: // d2f
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = F;
            value.context_value.f = (float)value.context_value.d;

            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.f)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x86: // i2f
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = F;
            value.context_value.f = (float)value.context_value.i;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.f)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x89: // l2f
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type = F;

            value.context_value.f = (float)value.context_value.j;

            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.f)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x88: // l2i
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = I;
            value.context_value.i = (int)value.context_value.j;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.i)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x8e: // d2i
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = I;
            value.context_value.i = (int)value.context_value.d;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.i)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x85: // i2l
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = J;
            value.context_value.j = (long)value.context_value.i;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.j)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x8f: // d2l
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = J;
            value.context_value.j = (long)value.context_value.d;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.j)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x63: // dadd
        case 0x62: // fadd
        case 0x60: // iadd
        case 0x61: // ladd
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto result = value1 + value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x98: // dcmp<op> dcmpg
        case 0x97: // dcmp<op> dcmpl
        {
            int i       = 1;
            int n       = *(byte)-0x97;
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto entry = std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&i)));
            if (value1->context_value.d > value2->context_value.d) {
                entry->context_value.i = 1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.d < value2->context_value.d) {
                entry->context_value.i = -1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.d > value2->context_value.d) {
                entry->context_value.i = 0;
                sf_local->operand_stack.push(entry);
            } else if (isnan(value1->context_value.d) ||
                       isnan(value2->context_value.d)) {
                if (n == 1) {
                    entry->context_value.i = 1;
                    sf_local->operand_stack.push(entry);
                } else if (n == 0) {
                    entry->context_value.i = -1;
                    sf_local->operand_stack.push(entry);
                }
            }
        } break;
        case 0xe: // dconst_<d> dconst_0
        case 0xf: // dconst_<d> dconst_1
        {
            double e = (double)*byte - 0xe;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", D, reinterpret_cast<void *>(&e))));

        } break;
        case 0x6f: // ddiv
        case 0x6e: // fdiv
        case 0x6d: // ldiv
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value2.context_value.d == 0) {
                throw std::runtime_error("ArithmeticException");
            } else {
                auto result = value1 / value2;
                sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                    new ContextEntry(std::move(result))));
            }
        } break;
        case 0x18: // dload
        case 0x16: // lload
        {
            int index        = -1;
            const int index1 = *(byte + 1);
            if (wide) {
                wide             = false;
                const int index2 = *(byte + 2);
                index            = (index1 << 8) | index2;
                byte++;
            } else {
                index = index1;
            }
            byte++;
            auto value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x26: // dload_0
        case 0x27: // dload_1x
        case 0x28: // dload_2
        case 0x29: // dload_3
        {
            auto index = *(byte)-0x26;
            auto value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x6b: // dmul
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value1.entry_type == B) {
                value1.entry_type      = D;
                value1.context_value.i = (double)value1.context_value.b;
            }
            if (value2.entry_type == B) {
                value2.entry_type      = D;
                value2.context_value.i = (double)value2.context_value.b;
            }
            auto result = value1 * value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x6a: // fmul
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value1.entry_type == B) {
                value1.entry_type      = F;
                value1.context_value.i = (float)value1.context_value.b;
            }
            if (value2.entry_type == B) {
                value2.entry_type      = F;
                value2.context_value.i = (float)value2.context_value.b;
            }
            auto result = value1 * value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x68: // imul
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value1.entry_type == B) {
                value1.entry_type      = I;
                value1.context_value.i = (int)value1.context_value.b;
            }
            if (value2.entry_type == B) {
                value2.entry_type      = I;
                value2.context_value.i = (int)value2.context_value.b;
            }
            auto result = value1 * value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x69: // lmul
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto result = value1 * value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x77: // dneg
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            double d = -1;
            auto result =
                value * ContextEntry("", D, reinterpret_cast<void *>(&d));
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x73: // drem
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto result =
                fmod(value1->context_value.d, value2->context_value.d);

            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", D, reinterpret_cast<void *>(&result))));
        } break;
        case 0xaf: // dreturn
        case 0xae: // freturn

        {
            auto retval = sf_local->operand_stack.top();
            while (!sf_local->operand_stack.empty()) {
                sf_local->operand_stack.pop();
            }
            return retval;
        } break;
        case 0x39: // dstore
        case 0x37: // lstore

        {
            int index        = -1;
            const int index1 = *(byte + 1);
            if (wide) {
                wide             = false;
                const int index2 = *(byte + 2);
                index            = index1 << 8 + index2;
                byte++;
            } else {
                index = index1;
                byte++;
            }
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (index > sf_local->lva.size()) {
                while (index > sf_local->lva.size()) {
                    sf_local->lva.push_back(std::make_shared<ContextEntry>());
                }
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else {
                sf_local->lva[index] = value;
            }
        } break;
        case 0x47: // dstore_0
        case 0x48: // dstore_1
        case 0x49: // dstore_2
        case 0x4a: // dstore_3
        {
            auto index = *(byte)-0x47;
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else if (index > sf_local->lva.size()) {
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            } else {
                sf_local->lva.erase(sf_local->lva.begin() + index);
                sf_local->lva.insert(sf_local->lva.begin() + index, value);
            }
            if ((index + 1) == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else if ((index + 1) > sf_local->lva.size()) {
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            } else {
                sf_local->lva.erase(sf_local->lva.begin() + index);
                sf_local->lva.insert(sf_local->lva.begin() + index, value);
            }

        } break;

        case 0x66: // fsub
        case 0x67: // dsub
        case 0x64: // isub
        case 0x65: // lsub
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            ContextEntry result = value1 - value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0x5a: // dup_x1
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            sf_local->operand_stack.push(value1);
            sf_local->operand_stack.push(value2);
            sf_local->operand_stack.push(value1);
        } break;
        case 0x5b: // dup_x2
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value2->entry_type == D) {
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            } else {
                auto value3 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value3);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            }
        } break;

        case 0x5c: // dup2
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (category(value1->entry_type) == 2) {
                auto value = value1;
                sf_local->operand_stack.push(value);
                sf_local->operand_stack.push(value);
            } else {
                auto value2 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            }
        } break;
        case 0x5d: // dup2_x1
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (category(value1->entry_type) == 2) {
                auto value2 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            } else {
                auto value2 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                auto value3 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value3);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            }
        } break;
        case 0x5e: // dup2_x2
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (category(value1->entry_type) == 2 &&
                category(value2->entry_type) ==
                    2) { // Form 4: value2, value1 -> value1, value2, value1
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value2);
                sf_local->operand_stack.push(value1);
            } else if (category(value1->entry_type) == 1 &&
                       category(value2->entry_type) == 1) {
                auto value3 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                if (category(value3->entry_type) ==
                    2) { // Form 3: value3, value2, value1 -> value2,
                         // value1, value3, value2, value1
                    sf_local->operand_stack.push(value2);
                    sf_local->operand_stack.push(value1);
                    sf_local->operand_stack.push(value3);
                    sf_local->operand_stack.push(value2);
                    sf_local->operand_stack.push(value1);
                } else { // Form 1
                    auto value4 = sf_local->operand_stack.top();
                    sf_local->operand_stack.pop();
                    if (category(value4->entry_type) == 1) {
                        sf_local->operand_stack.push(value2);
                        sf_local->operand_stack.push(value1);
                        sf_local->operand_stack.push(value4);
                        sf_local->operand_stack.push(value3);
                        sf_local->operand_stack.push(value2);
                        sf_local->operand_stack.push(value1);
                    }
                }
            } else if (category(value1->entry_type) == 2 &&
                       category(value2->entry_type) == 1) {
                auto value3 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                if (value3->entry_type ==
                    1) { // Form 2: value3, value2, value1 →
                         // value1, value3, value2, value1
                    sf_local->operand_stack.push(value1);
                    sf_local->operand_stack.push(value3);
                    sf_local->operand_stack.push(value2);
                    sf_local->operand_stack.push(value1);
                }
            }
        } break;
        case 0x87: // i2d
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = D;
            value.context_value.d = (double)value.context_value.i;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.d)));
            sf_local->operand_stack.push(valptr);

        } break;
        case 0x8d: // f2d
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = D;
            value.context_value.d = (double)value.context_value.f;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.d)));
            sf_local->operand_stack.push(valptr);

        } break;
        case 0x8a: // l2d
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = D;
            value.context_value.d = (double)value.context_value.j;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.d)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x8b: // f2i
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = I;
            value.context_value.i = (int)value.context_value.f;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.i)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x8c: // f2l
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value.entry_type      = J;
            value.context_value.j = (long)value.context_value.f;
            std::shared_ptr<ContextEntry> valptr(new ContextEntry(
                "", value.entry_type,
                reinterpret_cast<void *>(&value.context_value.j)));
            sf_local->operand_stack.push(valptr);
        } break;
        case 0x96: // fcmpg
        case 0x95: // fcmpl
        {
            int i       = 1;
            int n       = *(byte)-0x95;
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto entry = std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&i)));
            if (value1->context_value.d > value2->context_value.d) {
                entry->context_value.i = 1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.d < value2->context_value.d) {
                entry->context_value.i = -1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.d == value2->context_value.d) {
                entry->context_value.i = 0;
                sf_local->operand_stack.push(entry);
            } else if (isnan(value1->context_value.d) ||
                       isnan(value2->context_value.d)) {
                if (n == 1) {
                    entry->context_value.i = 1;
                    sf_local->operand_stack.push(entry);
                } else if (n == 0) {
                    entry->context_value.i = -1;
                    sf_local->operand_stack.push(entry);
                }
            }
        } break;
        case 0xb: // fconst_0
        case 0xc: // fconst_1
        case 0xd: // fconst_2
        {
            float e    = static_cast<float>(static_cast<int>(*byte - 0xb));
            auto entry = std::shared_ptr<ContextEntry>(
                new ContextEntry("", F, reinterpret_cast<void *>(&e)));
            sf_local->operand_stack.push(entry);
        } break;
        case 0x17: // fload
        case 0x15: // iload
        {
            int index        = -1;
            const int index1 = *(++byte);
            if (wide) {
                wide             = false;
                const int index2 = *(++byte);
                index            = index1 << 8 + index2;
            } else {
                index = index1;
            }
            auto value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x22: // fload_0
        case 0x23: // fload_1
        case 0x24: // fload_2
        case 0x25: // fload_3
        {
            auto index = *(byte)-0x22;
            auto value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x76: // fneg
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            float f = -1;
            auto result =
                value * ContextEntry("", F, reinterpret_cast<void *>(&f));
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));

        } break;
        case 0x72: // frem
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto result =
                fmod(value1->context_value.f, value2->context_value.f);

            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", F, reinterpret_cast<void *>(&result))));
        } break;
        case 0x38: // fstore
        case 0x36: // istore
        {
            int index        = -1;
            const int index1 = *(++byte);
            if (wide) {
                wide             = false;
                const int index2 = *(++byte);
                index            = index1 << 8 + index2;
            } else {
                index = index1;
            }
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            while (index > sf_local->lva.size()) {
                sf_local->lva.push_back(std::make_shared<ContextEntry>());
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else {
                sf_local->lva[index] = value;
            }
        } break;
        case 0x43: // fstore_0
        case 0x44: // fstore_1
        case 0x45: // fstore_2
        case 0x46: // fstore_3
        {
            auto index = *(byte)-0x43;
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            while (index > sf_local->lva.size()) {
                sf_local->lva.push_back(std::make_shared<ContextEntry>());
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else {
                sf_local->lva[index] = value;
            }
        } break;
        case 0xb4: // getfield
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            int index       = (indexbyte1 << 8) | indexbyte2;
            auto objref     = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto field_name = cp.at(class_name)->getFieldByIndex(index);
            if (objref->isNull)
                throw std::runtime_error("NullPointerException");
            auto value = objref->cf.at(field_name);
            sf_local->operand_stack.push(value);
        } break;
        case 0xb2: // getstatic
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            auto index      = (indexbyte1 << 8) | indexbyte2;
            auto field_name = cp.at(class_name)->getFieldByIndex(index);
            if (cf->at(class_name).find(field_name) !=
                cf->at(class_name).end()) {
                sf_local->operand_stack.push(cf->at(class_name).at(field_name));
            }
        } break;
        case 0xa7: // goto
        {
            unsigned short int branchbyte1 = *(byte + 1);
            unsigned short int branchbyte2 = *(byte + 2);

            short int offset = (signed int)(branchbyte1 << 8) | branchbyte2;
            byte += offset;
            byte--;
        } break;
        case 0xc8: // goto_w
        {
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);
            auto branchbyte3 = *(++byte);
            auto branchbyte4 = *(++byte);

            auto offset = (branchbyte1 << 24) | (branchbyte2 << 16) |
                          (branchbyte3 << 8) | branchbyte4;

            byte += offset;
        } break;
        case 0x91: // i2b
        case 0x92: // i2c
        {
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value->entry_type = C;
            sf_local->operand_stack.push(value);
        } break;
        case 0x93: // i2s
        {
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            value->entry_type = S;
            sf_local->operand_stack.push(value);
        } break;
        case 0x7e: // iand
        case 0x7f: // land
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto result = value1 & value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;

        case 0x6c: // idiv
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value2.context_value.i == 0) {
                throw std::runtime_error("ArithmeticException");
            } else {
                auto value3 = value1 / value2;
                sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                    new ContextEntry(std::move(value3))));
            }
        } break;
        case 0x2: // iconst_m1
        case 0x3: // iconst_0
        case 0x4: // iconst_1
        case 0x5: // iconst_2
        case 0x6: // iconst_3
        case 0x7: // iconst_4
        case 0x8: // iconst_5
        {
            int e = static_cast<int>(*byte - 0x3);
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&e))));
        } break;
        case 0xa5: // if_acmpeq
        case 0xa6: // if_acmpne
        {
            auto index  = *(byte)-0xa5;
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);
            if (index == 0) {
                if (value1.l == value2.l) {
                    auto offset = (branchbyte1 << 8) | branchbyte2;
                    byte += offset;
                }
            } else if (index == 1) {
                if (value1.l != value2.l) {
                    auto offset = (branchbyte1 << 8) | branchbyte2;
                    byte += offset;
                }
            }
        } break;
        case 0x9f: // if_icmpeq
        case 0xa0: // if_icmpne
        case 0xa1: // if_icmplt
        case 0xa2: // if_icmpge
        case 0xa3: // if_icmpgt
        case 0xa4: // if_icmple
        {
            auto index  = *(byte)-0x9f;
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto branchbyte1 = *(byte + 1);
            auto branchbyte2 = *(byte + 2);
            int offset       = 0;
            if (index == 0) { // if_icmpeq
                if (value1->context_value.b == value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            } else if (index == 1) { // if_icmpne
                if (value1->context_value.b != value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            } else if (index == 2) { // if_icmplt
                if (value1->context_value.b < value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            } else if (index == 3) { // if_icmpge
                if (value1->context_value.b >= value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            } else if (index == 4) { // if_ifmpgt
                if (value1->context_value.b > value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            } else if (index == 5) { // if_icmple
                if (value1->context_value.b <= value2->context_value.b) {
                    offset = (branchbyte1 << 8) | branchbyte2;
                    byte--;
                }
            }
            if (!offset)
                byte += 2;
            byte += offset;

        } break;
        case 0x99: // ifeq
        case 0x9a: // ifne
        case 0x9b: // iflt
        case 0x9c: // ifge
        case 0x9d: // ifgt
        case 0x9e: // ifle
        {
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            int n = *(byte)-0x99;
            if (n == 0) { // ifeq
                if (!value->context_value.i) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            } else if (n == 1) { // ifne
                if (value->context_value.i) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            } else if (n == 2) { // iflt
                if (value->context_value.i < 0) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            } else if (n == 3) { // ifge
                if (value->context_value.i >= 0) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            } else if (n == 4) { // ifgt
                if (value->context_value.i > 0) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            } else if (n == 5) { // ifle
                if (value->context_value.i <= 0) {
                    auto branchbyte1 = *(byte + 1);
                    auto branchbyte2 = *(byte + 2);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                    byte--;
                } else {
                    byte++;
                }
            }
        }

        break;
        case 0xc6: // ifnull
        case 0xc7: // ifnonnull
        {
            auto pc    = byte - 1;
            auto index = *(byte)-0xc6;
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);
            int offset       = (branchbyte1 << 8) | branchbyte2;

            if (index == 0) {
                if (value.isNull) {
                    byte = pc + offset;
                }
            } else {
                if (!value.isNull) {
                    byte = pc + offset;
                }
            }
        } break;
        case 0x84: // iinc
        {
            int index   = -1;
            auto index1 = *(++byte);
            if (wide) {
                auto index2 = *(++byte);
                index       = (index1 << 8) | index2;
            } else {
                index = index1;
            }

            int constant   = 0;
            char constant1 = *(++byte);
            if (wide) {
                char constant2 = *(++byte);
                constant       = (constant1 << 8) | constant2;
                wide           = false;
            } else {
                constant = constant1;
            }
            sf_local->lva[index]->context_value.i += constant;

        } break;
        case 0xc1: // instanceof
        {
            // I'm not sure about if this will work or not;
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto objref        = sf_local->operand_stack.top();
            int zero           = 0;
            int one            = 1;
            sf_local->operand_stack.pop();
            if (objref->isNull) {
                // push 0 into the stack
                sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                    new ContextEntry("", I, reinterpret_cast<void *>(&zero))));
            } else {
                if (objref->class_name ==
                    cp.at(class_name)->getNameByIndex(index)) {
                    sf_local->operand_stack.push(
                        std::shared_ptr<ContextEntry>(new ContextEntry(
                            "", I, reinterpret_cast<void *>(&one))));
                } else {
                    sf_local->operand_stack.push(
                        std::shared_ptr<ContextEntry>(new ContextEntry(
                            "", I, reinterpret_cast<void *>(&zero))));
                }
            }
        } break;
        case 0x1a: // iload_0
        case 0x1b: // iload_1
        case 0x1c: // iload_2
        case 0x1d: // iload_3
        {
            auto index = *(byte)-0x1a;

            std::shared_ptr<ContextEntry> value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);
        } break;
        case 0x74: // ineg
        {
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            if (value->entry_type == B) {
                value->entry_type      = I;
                value->context_value.i = (int)value->context_value.b;
            }

            int i = -1;
            auto result =
                *value * ContextEntry("", I, reinterpret_cast<void *>(&i));
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0xba: // invokedynamic
            break;
        case 0xb9: // invokeinterface
            break;
        case 0x80: // ior
        case 0x81: // lor
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto result = value1 || value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));

        } break;
        case 0x70: // irem
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto result = value1.context_value.i % value2.context_value.i;

            if (value1.context_value.i == 0) {
                throw std::runtime_error("ArithmeticException");
            }
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&result))));
        } break;
        case 0xac: // ireturn
        case 0xad: // lreturn
        {
            auto retval = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            while (!sf_local->operand_stack.empty()) {
                sf_local->operand_stack.pop();
            }

            return retval;
        } break;
        case 0x78: // ishl
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            int value2 = sf_local->operand_stack.top()->context_value.i & 0x1f;
            sf_local->operand_stack.pop();

            auto result = value1.context_value.i << value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&result))));
        } break;
        case 0x7a: // ishr
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            int value2 = sf_local->operand_stack.top()->context_value.i & 0x1f;
            sf_local->operand_stack.pop();
            ContextEntry("", I, static_cast<void *>(&value1));
            auto result = value1.context_value.i >> value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&result))));
        } break;
        case 0xb8: // invokestatic
        case 0xb7: // invokespecial
        case 0xb6: // invokevirtual
        {
            auto old_lva       = sf_local->lva;
            auto invokeType    = *(byte);
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto class_name_at_cp =
                cp.at(class_name)->getClassNameFromMethodByIndex(index);
            std::shared_ptr<ContextEntry> exec_return;
            auto cm_index    = cp.at(class_name)->getMethodNameIndex(index);
            auto method_name = cp.at(class_name)->getMethodNameByIndex(index);

            if (class_name_at_cp.find("java/lang/StringBuilder", 0) ==
                std::string::npos) {
                if (cm_index == -1) {
                    // init method from java object no need to call
                    break;
                }
                auto cmval = *cm;
                cmval.find(class_name);
                if (cm_index == -2) {
                    auto name_and_type =
                        cp.at(class_name)->getNameAndTypeByIndex(index);
                    auto found = name_and_type.find("println");
                    if (found != std::string::npos) {
                        auto args = std::string(
                            name_and_type.begin() +
                                name_and_type.find_first_of('(') + 1,
                            name_and_type.begin() +
                                name_and_type.find_first_of(')'));
                        auto args_size = countArgs(args);
                        std::vector<std::shared_ptr<ContextEntry>> prints(
                            args_size);
                        if (args_size > 0) {
                            for (auto &print : prints) {
                                print = sf_local->operand_stack.top();
                                sf_local->operand_stack.pop();
                            }
                            for (auto it = prints.end() - 1;
                                 it >= prints.begin(); it--) {
                                it->get()->PrintValue();
                                std::cout << std::endl;
                            }
                        } else {
                            std::cout << std::endl;
                        }
                    }
                } else if (cm_index > 0) {
                    cm_index = cp.at(class_name_at_cp)
                                   ->getMethodIndexByName(method_name);
                    auto args_length =
                        getArgsLen(class_name_at_cp, method_name);
                    auto a = cm->at(class_name_at_cp);
                    std::vector<unsigned char> code(
                        a.at(cm_index).attributes[0].code);
                    std::vector<std::shared_ptr<ContextEntry>> lva;
                    for (int i = 0; i < args_length; i++) {
                        lva.push_back(sf_local->operand_stack.top());
                        sf_local->operand_stack.pop();
                    }
                    if (invokeType != 0xb8) {
                        // case static we dont need to get reference from stack
                        auto objectRef = sf_local->operand_stack.top();
                        lva.push_back(objectRef);
                        sf_local->operand_stack.pop(); // object ref
                    }
                    std::reverse(lva.begin(), lva.end());
                    auto old_os         = sf_local->operand_stack;
                    auto old_class_name = class_name;
                    class_name          = class_name_at_cp;
                    if (method_name == "addCarta") {
                        std::cout << "here";
                    }
                    exec_return             = Exec(code, &lva);
                    class_name              = old_class_name;
                    sf_local->operand_stack = old_os;
                }
                if (exec_return != nullptr) {
                    if (exec_return->isReturnAddress()) {
                        byte = bytecode.begin() + exec_return->context_value.i;
                    } else {
                        sf_local->operand_stack.push(exec_return);
                    }
                }
            } else {
                if (method_name == "<init>")
                    str = "";
                else if (method_name == "append") {
                    auto str_to_append =
                        sf_local->operand_stack.top()->string_instance;
                    sf_local->operand_stack.pop();
                    str += str_to_append;
                    sf_local->operand_stack.push(std::make_shared<ContextEntry>(
                        "", R, reinterpret_cast<void *>(&str)));
                } else if (method_name == "println") {
                    std::cout << sf_local->operand_stack.top()->string_instance
                              << std::endl;
                    sf_local->operand_stack.pop();
                }
            }
        } break;
        case 0x3b: // istore_0
        case 0x3c: // istore_1
        case 0x3d: // istore_2
        case 0x3e: // istore_3
        {
            auto index = *(byte)-0x3b;
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto lva_size = sf_local->lva.size();
            if (index > lva_size) {
                while (index > lva_size) {
                    sf_local->lva.push_back(std::make_shared<ContextEntry>());
                    lva_size = sf_local->lva.size();
                }
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else {
                sf_local->lva[index] = value;
            }
        } break;
        case 0x7c: // iushr
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top()->context_value.i & 0x1f;
            sf_local->operand_stack.pop();

            auto result = value1->context_value.i >> value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&result))));
        } break;
        case 0x82: // ixor
        case 0x83: // lxor
        {
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto result = value1 ^ value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0xa8: // jsr
        {
            int index = (++byte) - bytecode.begin();
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(index))));
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);

            auto offset         = (branchbyte1 << 8) | branchbyte2;
            int nextInstruction = ++byte - bytecode.begin();
            auto ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                "", I, reinterpret_cast<void *>(nextInstruction)));
            ce->setAsRetAddress();
            sf_local->operand_stack.push(ce);
            byte += offset;

        } break;
        case 0xc9: // jsr_w
        {
            int index = (++byte) - bytecode.begin();
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(index))));
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);
            auto branchbyte3 = *(++byte);
            auto branchbyte4 = *(++byte);

            auto offset = (branchbyte1 << 24) | (branchbyte2 << 16) |
                          (branchbyte3 << 8) | branchbyte4;
            int nextInstruction = ++byte - bytecode.begin();
            auto ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                "", I, reinterpret_cast<void *>(nextInstruction)));
            ce->setAsRetAddress();
            sf_local->operand_stack.push(ce);
            byte += offset;
        } break;
        case 0x94: // lcmp
        {
            int i       = 1;
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto entry = std::shared_ptr<ContextEntry>(
                new ContextEntry("", I, reinterpret_cast<void *>(&i)));
            if (value1->context_value.j > value2->context_value.j) {
                entry->context_value.i = 1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.j < value2->context_value.j) {
                entry->context_value.i = -1;
                sf_local->operand_stack.push(entry);
            } else if (value1->context_value.j == value2->context_value.j) {
                entry->context_value.i = 0;
                sf_local->operand_stack.push(entry);
            }
        } break;
        case 0x9: // lconst_0
        case 0xa: // lconst_1
        {
            long e = (long)*byte - 0x9;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", J, reinterpret_cast<void *>(&e))));

        } break;
        case 0x12: // ldc
        {
            auto index       = *(++byte);
            auto intfloatref = cp.at(class_name)->getValueByIndex(index);
            std::shared_ptr<ContextEntry> ce;
            if (intfloatref.t == R) {
                ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                    "java/lang/String", R,
                    reinterpret_cast<void *>(&intfloatref.str_value)));
            } else {
                ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                    class_name, intfloatref.t,
                    reinterpret_cast<void *>(&intfloatref.val)));
            }
            sf_local->operand_stack.push(ce);
        } break;
        case 0x13: // ldc_w
        {
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);
            auto index       = (branchbyte1 << 8) | branchbyte2;
            auto intfloatref = cp.at(class_name)->getValueByIndex(index);
            std::shared_ptr<ContextEntry> ce;
            if (intfloatref.t == R) {
                ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                    "", R, reinterpret_cast<void *>(&intfloatref.str_value)));
            } else {
                ce = std::shared_ptr<ContextEntry>(new ContextEntry(
                    "", intfloatref.t,
                    reinterpret_cast<void *>(&intfloatref.val)));
            }
            sf_local->operand_stack.push(ce);
        } break;
        case 0x14: // ldc2_w
        {
            auto index1   = *(++byte);
            auto index2   = *(++byte);
            auto index    = (index1 << 8) | index2;
            DoubleLong dl = cp.at(class_name)->getNumberByIndex(index);
            auto cte      = std::shared_ptr<ContextEntry>(
                new ContextEntry("", dl.t, reinterpret_cast<void *>(&dl.val)));
            sf_local->operand_stack.push(cte);
        } break;
        case 0x1e: // lload_0
        case 0x1f: // lload_1
        case 0x20: // lload_2
        case 0x21: // lload_3
        {
            auto index = *(byte)-0x1e;
            auto value = sf_local->lva.at(index);
            sf_local->operand_stack.push(value);

        } break;
        case 0x75: // lneg
        {
            auto value = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            long j      = -1 * value.context_value.j;
            auto result = ContextEntry("", J, reinterpret_cast<void *>(&j));
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry(std::move(result))));
        } break;
        case 0xab: // lookupswitch
        case 0x71: // lrem
        {
            auto value2 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();

            auto result = value1.context_value.j % value2.context_value.j;

            if (value1.context_value.j == 0) {
                throw std::runtime_error("ArithmeticException");
            }
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", J, reinterpret_cast<void *>(&result))));

        } break;

        case 0x79: // lshl
        {
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            int sll = value2->context_value.j;

            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            long int operand = static_cast<long int>(value1.context_value.j);

            auto result = value1.context_value.j << sll;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", J, reinterpret_cast<void *>(&result))));
        } break;
        case 0x7b: // lshr
        {
            auto value2 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            int srl = value2->context_value.j;

            auto value1 = *sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            long int operand = static_cast<long int>(value1.context_value.j);

            auto result = value1.context_value.j >> srl;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", J, reinterpret_cast<void *>(&result))));
        } break;
        case 0x3f: // lstore_0
        case 0x40: // lstore_1
        case 0x41: // lstore_2
        case 0x42: // lstore_3
        {
            auto index = *(byte)-0x3f;
            auto value = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto lva_size = sf_local->lva.size();
            if (index > lva_size) {
                while (index > lva_size) {
                    sf_local->lva.push_back(std::make_shared<ContextEntry>());
                    lva_size = sf_local->lva.size();
                }
            }
            if (index == sf_local->lva.size()) {
                sf_local->lva.push_back(value);
            } else {
                sf_local->lva[index] = value;
            }
        } break;
        case 0x7d: // lushr
        {
            auto value1 = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto value2 = sf_local->operand_stack.top()->context_value.i & 0x3f;
            sf_local->operand_stack.pop();

            auto result = value1->context_value.i >> value2;
            sf_local->operand_stack.push(std::shared_ptr<ContextEntry>(
                new ContextEntry("", J, reinterpret_cast<void *>(&result))));
        } break;
        case 0xc2: // monitorenter
        case 0xc3: // monitorexit
            break;
        case 0xc5: // multianewarray
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            int index       = (indexbyte1 << 8) | indexbyte2;
            auto dimensions = *(++byte);
            auto array_desc = cp.at(class_name)->getNameByIndex(index);
            std::vector<unsigned int> counters;
            for (auto i = 0; i < dimensions; i++) {
                counters.push_back(
                    sf_local->operand_stack.top()->context_value.i);
                sf_local->operand_stack.pop();
            }
            auto type_index  = array_desc.find_first_not_of('[');
            std::string type = {array_desc.at(type_index)};
            std::shared_ptr<ContextEntry> array_operator = nullptr;
            array_operator =
                std::make_shared<ContextEntry>("", TypeMap.at(type), 1);
            std::shared_ptr<ContextEntry> init = array_operator;
            for (auto i = 0; i < dimensions - 1; i++) {
                auto newarray =
                    std::make_shared<ContextEntry>("", TypeMap.at(type), 1);
                array_operator->addToArray(0, newarray);
                array_operator = std::move(array_operator->arrayRef[0]);
            }
            sf_local->operand_stack.push(init);
        } break;
        case 0xbc: // newarray
        {
            auto atype = static_cast<int>(*(++byte));
            auto count = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            std::shared_ptr<ContextEntry> ce(new ContextEntry(
                "", ATypeMap.at(atype), count->context_value.b));
            sf_local->operand_stack.push(ce);
        } break;
        case 0x0: // nop
            break;

        case 0x57: // pop
        {
            if (category(sf_local->operand_stack.top()->entry_type) == 1) {
                sf_local->operand_stack.pop();
            }
        } break;
        case 0x58: // pop2
        {
            if (category(sf_local->operand_stack.top()->entry_type) == 2) {
                sf_local->operand_stack.pop();
            } else if (category(sf_local->operand_stack.top()->entry_type) ==
                       1) {
                sf_local->operand_stack.pop();
                sf_local->operand_stack.pop();
            }
        } break;
        case 0xb5: // putfield
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            auto index      = (indexbyte1 << 8) | indexbyte2;
            auto field_name = cp.at(class_name)->getFieldByIndex(index);
            auto value      = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            auto objRef = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            objRef->cf[field_name] = value;
        } break;
        case 0xb3: // putstatic
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            auto index      = (indexbyte1 << 8) | indexbyte2;
            auto field_name = cp.at(class_name)->getFieldByIndex(index);
            auto value      = sf_local->operand_stack.top();
            sf_local->operand_stack.pop();
            cf->operator[](class_name)[field_name] = value;

        } break;
        case 0xa9: // ret
        {
            auto index1        = *(++byte);
            unsigned int index = -1;
            if (wide) {
                wide        = false;
                auto index2 = *(++byte);
                index       = (index1 << 8) | (index2);
            } else {
                index = index1;
            }
            auto loadedValue = sf_local->lva.at(index);
            loadedValue->setAsRetAddress();

            return loadedValue;
        } break;
        case 0xb1: // return
        {
            while (!sf_local->operand_stack.empty())
                sf_local->operand_stack.pop();
        } break;
        case 0x11: // sipush
        {
            auto byte1  = *(++byte);
            auto byte2  = *(++byte);
            auto short_ = (byte1 << 8) | byte2;
            std::shared_ptr<ContextEntry> ce(
                new ContextEntry("", I, reinterpret_cast<void *>(&short_)));
            sf_local->operand_stack.push(ce);
        } break;
        case 0x5f: // swap
        {
            if (category(sf_local->operand_stack.top()->entry_type) == 1) {
                auto value1 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                auto value2 = sf_local->operand_stack.top();
                sf_local->operand_stack.pop();
                sf_local->operand_stack.push(value1);
                sf_local->operand_stack.push(value2);
            }
        } break;
        case 0xaa: { // tableswitch
            auto inital_pc      = byte;
            int tableswitchline = i;
            int padding         = i;
            auto index1         = sf_local->operand_stack.top();
            auto index = static_cast<signed int>(index1->context_value.b);
            while (padding % 4 != 0) {
                byte++; // removes padding bytes (up to 3)
                padding++;
            }
            signed int default_ = 0;
            for (int k = 0; k < 4; k++) { // gets default value
                auto default1 = static_cast<unsigned short int>(*byte);
                int sll       = 24 - (8 * k);
                default_      = (default1 << sll) | default_;
                byte++;
            }

            signed int low = 0;
            for (int k = 0; k < 4; k++) { // gets low value
                auto lowbyte = static_cast<unsigned short int>(*byte);
                int sll      = 24 - (8 * k);
                low          = (lowbyte << sll) | low;
                byte++;
            }

            signed int high = 0;
            for (int k = 0; k < 4; k++) { // gets high value
                auto highbyte = static_cast<unsigned short int>(*byte);
                int sll       = 24 - (8 * k);
                high          = (highbyte << sll) | high;
                byte++;
            }

            if ((index < low) || (index > high)) { // sends to default address
                byte = inital_pc;
                byte += default_;
            } else {
                int k = 0;
                std::map<unsigned int, signed int> jumptable;
                while (k < high) {
                    int jumpoffset = 0;
                    for (int l = 0; l < 4; l++) { // gets offset
                        auto value = static_cast<unsigned short int>(*byte);
                        int sll = 24 - (8 * l); // value = byte1<<24 | byte2<<16
                                                // | byte3<<8 | byte4<<0
                        jumpoffset = (value << sll) | jumpoffset;
                        byte++;
                    }
                    jumptable.insert(std::make_pair(k + 1, jumpoffset));
                    k++;
                }
                byte--;
                int op = jumptable.at(1);
                byte -= op; // voltar pc para endereco de tableswitch
                int offset = jumptable.at(index);
                byte += offset;
            }
        } break;
        case 0xc4: // wide
        {
            wide = true;
        } break;
        default:
            break;
        }
    }
    return nullptr;
}

unsigned int MethodExecuter::countArgs(std::string argument_str) {
    unsigned int args_number = 0;
    for (auto arg = argument_str.begin(); arg < argument_str.end(); arg++) {
        switch (*arg) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'J':
        case 'S':
        case 'Z':
            args_number++;
            break;
        case 'L': {
            auto end_arg_pos = argument_str.find_first_of(
                ";", std::distance(argument_str.begin(), arg));
            args_number++;
            arg += end_arg_pos;
        } break;
        default:
            break;
        }
    }
    return args_number;
}