#include <MethodExecuter/MethodExecuter.hpp>
#include <math.h>

MethodExecuter::MethodExecuter(ConstantPool *cp, ClassMethods cm) {
    this->cp = cp;
    this->cm = cm;
}

ContextEntry MethodExecuter::Exec(std::vector<unsigned char> bytecode,
                                  std::vector<ContextEntry *> ce) {
    sf = new StackFrame(ce);
    std::vector<int> args;
    int args_counter = 0;
    bool wide        = false;
    for (auto byte = bytecode.begin(); byte != bytecode.end(); byte++) {
        switch (*byte) {
        case 0x32: // aaload
        {
            auto index = sf->operand_stack.top()->context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top()->isArray) {
                auto arrayRef = sf->operand_stack.top()->getArray();
                sf->operand_stack.pop();
                if (index >= arrayRef.size()) {
                    throw std::runtime_error("ArrayIndexOutOfBoundsException");
                }
                sf->operand_stack.push(arrayRef.at(index).get());
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x53: // aastore
        {
            auto value = sf->operand_stack.top();
            std::shared_ptr<ContextEntry> value_ref(
                new ContextEntry(std::move(*value)));
            sf->operand_stack.pop();
            auto index = sf->operand_stack.top()->context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top()->isArray) {
                sf->operand_stack.top()->addToArray(index, value_ref);
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x01: // aconst_null
        {
            auto nil = ContextEntry();
            sf->operand_stack.push(&nil);
        } break;
        case 0x19: // aload
        {
            int index        = -1;
            const int index1 = *(byte + 1);
            if (wide) {
                const int index2 = *(byte + 2);
                index            = index1 << 8 + index2;
                byte++;
            } else {
                index = index1;
            }
            byte++;
            auto load = sf->lva.at(index);
            if (load->isReference()) {
                sf->operand_stack.push(load);
            } else {
                throw std::runtime_error("aload of non-reference object");
            }
        }
        case 0x2a: // aload_0
        case 0x2b: // aload_1
        case 0x2c: // aload_2
        case 0x2d: // aload_3
        {
            int index = *byte - 0x2a;
            auto load = sf->lva.at(index);
            if (load->isReference()) {
                sf->operand_stack.push(load);
            } else {
                throw std::runtime_error("aload of non-reference object");
            }
        } break;
        case 0xbd: // anewarray
        {
            int count = sf->operand_stack.top()->context_value.i;
            sf->operand_stack.pop();
            int index        = -1;
            const int index1 = *(byte + 1);
            if (wide) {
                const int index2 = *(byte + 2);
                index            = index1 << 8 + index2;
                byte++;
            } else {
                index = index1;
            }
            byte++;
            if (count < 0) {
                throw std::runtime_error("NegativeArraySizeException");
            }
            // do we need to save this into a heap? Idk
            sf->operand_stack.push(&ContextEntry(L, count));
        } break;
        case 0xb0: // areturn
        {
            auto retval = sf->operand_stack.top();
            while (!sf->operand_stack.empty()) {
                sf->operand_stack.pop();
            }
            if (!retval->isReference())
                throw std::runtime_error(
                    "areturn cannot return a non-reference value");
            return *retval;
        } break;
        case 0xbe: // arraylength
        {
            auto arrRef = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (!arrRef->isReference()) {
                std::runtime_error(
                    "arraylength called over a non-reference object");
            }

            auto length = arrRef->arrayLength();
            sf->operand_stack.push(&length);
        } break;
        case 0x3a: // astore
        {
            int index = *(byte + 1);
            byte++;
            auto objRef = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (!objRef->isReference()) {
                std::runtime_error("astore called over a non-reference object");
            }
            // return address type??
            sf->lva[index] = objRef;
        } break;
        case 0x4b: // astore_0
        case 0x4c: // astore_1
        case 0x4d: // astore_2
        case 0x4e: // astore_3
        {
            unsigned int index = *byte - 0x4b;
            auto objRef        = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (!objRef->isReference()) {
                std::runtime_error("astore called over a non-reference object");
            }
            sf->lva[index] = objRef;
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
            std::string className = cp->getNameByIndex(classNameIndex);
            auto cevoid           = reinterpret_cast<void *>(&sf->lva);
            auto entry            = ContextEntry(className, L, cevoid);
            sf->operand_stack.push(&entry);
            byte += 2;
        } break;
        case 0x59: // dup
        {
            auto top = sf->operand_stack.top();
            sf->operand_stack.push(top);
        } break;
        case 0x33: // baload
        case 0x34: // caload
        case 0x31: // daload
        case 0x30: // faload
        case 0x2e: // iaload
        {
            auto index = sf->operand_stack.top()->context_value.i;
            sf->operand_stack.pop();
            auto arrayref = sf->operand_stack.top()->getArray();
            sf->operand_stack.pop();

            if (arrayref.size() >= index)
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            auto value = arrayref.at(index);
            sf->operand_stack.push(value.get());
        } break;
        case 0x54: // bastore
        case 0x55: // castore
        case 0x52: // dastore
        case 0x51: // fastore
        case 0x4f: // iastore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index = sf->operand_stack.top()->context_value.i;
            sf->operand_stack.pop();
            auto arrayref = sf->operand_stack.top()->getArray();
            sf->operand_stack.pop();

            std::shared_ptr<ContextEntry> value_ref(
                new ContextEntry(std::move(*value)));
            arrayref[index] = value_ref;
            if (arrayref.size() >= index)
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            auto nvalue = arrayref.at(index).get();
            sf->operand_stack.push(nvalue);
        } break;
        case 0x10: // bipush
        {
            auto value = *(byte + 1);
            byte++;
            auto entry = ContextEntry("", B, reinterpret_cast<void *>(&value));
            sf->operand_stack.push(&entry);

        } break;

        case 0xc0: // checkcast
        {
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto objref        = sf->operand_stack.top();
            if (!objref->isNull) {
                sf->operand_stack.pop();
                if (objref->class_name == cp->getNameByIndex(index)) {
                    sf->operand_stack.push(objref);
                } else {
                    throw std::runtime_error("ClassCastException");
                }
            }
        } break;
        case 0x90: // d2f
        case 0x86: // i2f
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = F;
            sf->operand_stack.push(value);
        } break;
        case 0x8e: // d2i
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = I;
            sf->operand_stack.push(value);
        } break;
        case 0x8f: // d2l
        case 0x85: // i2l
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = L;
            sf->operand_stack.push(value);
        } break;
        case 0x63: // dadd
        case 0x62: // fadd
        case 0x60: // iadd
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto result = value1 + value2;
            sf->operand_stack.push(&result);
        } break;
        case 0x98: // dcmp<op> dcmpg
        case 0x97: // dcmp<op> dcmpl
        {
            int i       = 1;
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto entry = ContextEntry("", I, reinterpret_cast<void *>(&i));
            if (value1->context_value.d > value2->context_value.d) {
                entry.context_value.i = 1;
                sf->operand_stack.push(&entry);
            } else if (value1->context_value.d < value2->context_value.d) {
                entry.context_value.i = -1;
                sf->operand_stack.push(&entry);
            } else if (value1->context_value.d == value2->context_value.d) {
                entry.context_value.i = 0;
                sf->operand_stack.push(&entry);
            } else if (isnan(value1->context_value.d) ||
                       isnan(value2->context_value.d)) {
                if (*byte == 0x98) {
                    entry.context_value.i = 1;
                    sf->operand_stack.push(&entry);
                } else if (*byte == 0x97) {
                    entry.context_value.i = -1;
                    sf->operand_stack.push(&entry);
                }
            }
        } break;
        case 0xe: // dconst_<d> dconst_0
        case 0xf: // dconst_<d> dconst_1
        {
            char e     = *byte - 0xe;
            auto entry = ContextEntry("", D, reinterpret_cast<void *>(&e));
            sf->operand_stack.push(&entry);

        } break;
        case 0x6f: // ddiv
        case 0x6e: // fdiv
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto result = value1 / value2;
            sf->operand_stack.push(&result);

        } break;
        case 0x18: // dload
        case 0x16: // llong
        {
            auto index = *(byte + 1);
            byte++;
            auto value = sf->lva.at(index);
            sf->operand_stack.push(value);
            sf->operand_stack.push(value);

        } break;
        case 0x26: // dload_0
        case 0x27: // dload_1
        case 0x28: // dload_2
        case 0x29: // dload_3
        {
            auto index = *(byte)-0x26;
            auto value = sf->lva.at(index);
            sf->operand_stack.push(value);
            sf->operand_stack.push(value);

        } break;

        case 0x6b: // dmul
        case 0x6a: // fmul
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto result = value1 * value2;
            sf->operand_stack.push(&result);
        }
        case 0x77: // dneg
        {
            auto value = *sf->operand_stack.top();
            sf->operand_stack.pop();
            double d = -1;
            auto result =
                value * ContextEntry("", D, reinterpret_cast<void *>(&d));
            sf->operand_stack.push(&result);
        } break;
        case 0x73: // drem
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            auto result =
                fmod(value1->context_value.d, value2->context_value.d);

            sf->operand_stack.push(
                &ContextEntry("", D, reinterpret_cast<void *>(&result)));
        } break;
        case 0xaf: // dreturn
        case 0xae: // freturn
        {
            auto retval = *sf->operand_stack.top();
            while (!sf->operand_stack.empty()) {
                sf->operand_stack.pop();
            }
            return retval;
        } break;
        case 0x39: // dstore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index         = *(++byte);
            sf->lva[index]     = value;
            sf->lva[index + 1] = value;
        } break;
        case 0x47: // dstore_0
        case 0x48: // dstore_1
        case 0x49: // dstore_2
        case 0x4a: // dstore_3
        {
            auto index = *(byte)-0x47;
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            sf->lva[index]     = value;
            sf->lva[index + 1] = value;

        } break;

        case 0x66: // fsub
        case 0x67: // dsub
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            ContextEntry result = value1 - value2;
            sf->operand_stack.push(&result);
        } break;
        case 0x5a: // dup_x1
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();
            sf->operand_stack.push(value1);
            sf->operand_stack.push(value2);
            sf->operand_stack.push(value1);
        } break;
        case 0x5b: // dup_x2
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (value2->entry_type == D) {
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
            } else {
                auto value3 = sf->operand_stack.top();
                sf->operand_stack.pop();
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value3);
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
            }
        } break;

        case 0x5c: // dup2
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (category(value1->entry_type) == 2) {
                auto value = value1;
                sf->operand_stack.push(value);
                sf->operand_stack.push(value);
            } else {
                auto value2 = sf->operand_stack.top();
                sf->operand_stack.pop();
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value2);
            }
        } break;
        case 0x5d: // dup2_x1
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (category(value1->entry_type) == 2) {
                auto value2 = sf->operand_stack.top();
                sf->operand_stack.pop();
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
            } else {
                auto value2 = sf->operand_stack.top();
                sf->operand_stack.pop();
                auto value3 = sf->operand_stack.top();
                sf->operand_stack.pop();
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value3);
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
            }
        } break;
        case 0x5e: // dup2_x2
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (category(value1->entry_type) == 2 &&
                category(value2->entry_type) ==
                    2) { // Form 4: value2, value1 -> value1, value2, value1
                sf->operand_stack.push(value1);
                sf->operand_stack.push(value2);
                sf->operand_stack.push(value1);
            } else if (category(value1->entry_type) == 1 &&
                       category(value2->entry_type) == 1) {
                auto value3 = sf->operand_stack.top();
                sf->operand_stack.pop();
                if (category(value3->entry_type) ==
                    2) { // Form 3: value3, value2, value1 -> value2, value1,
                         // value3, value2, value1
                    sf->operand_stack.push(value2);
                    sf->operand_stack.push(value1);
                    sf->operand_stack.push(value3);
                    sf->operand_stack.push(value2);
                    sf->operand_stack.push(value1);
                } else { // Form 1
                    auto value4 = sf->operand_stack.top();
                    sf->operand_stack.pop();
                    if (category(value4->entry_type) == 1) {
                        sf->operand_stack.push(value2);
                        sf->operand_stack.push(value1);
                        sf->operand_stack.push(value4);
                        sf->operand_stack.push(value3);
                        sf->operand_stack.push(value2);
                        sf->operand_stack.push(value1);
                    }
                }
            } else if (category(value1->entry_type) == 2 &&
                       category(value2->entry_type) == 1) {
                auto value3 = sf->operand_stack.top();
                sf->operand_stack.pop();
                if (value3->entry_type ==
                    1) { // Form 2: value3, value2, value1 →
                         // value1, value3, value2, value1
                    sf->operand_stack.push(value1);
                    sf->operand_stack.push(value3);
                    sf->operand_stack.push(value2);
                    sf->operand_stack.push(value1);
                }
            }
        } break;

        case 0x8d: // f2d
        case 0x87: // i2d
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = D;
            sf->operand_stack.push(value);

        } break;
        case 0x8b: // f2i
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = I;
            sf->operand_stack.push(value);

        } break;
        case 0x8c: // f2l
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = L;
            sf->operand_stack.push(value);

        } break;
        case 0x96: // fcmpg
        case 0x95: // fcmppl
        {
            int i       = 1;
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto entry = ContextEntry("", I, reinterpret_cast<void *>(&i));
            if (value1->context_value.d > value2->context_value.d) {
                entry.context_value.i = 1;
                sf->operand_stack.push(&entry);
            } else if (value1->context_value.d < value2->context_value.d) {
                entry.context_value.i = -1;
                sf->operand_stack.push(&entry);
            } else if (value1->context_value.d == value2->context_value.d) {
                entry.context_value.i = 0;
                sf->operand_stack.push(&entry);
            } else if (isnan(value1->context_value.d) ||
                       isnan(value2->context_value.d)) {
                if (*byte == 0x96) {
                    entry.context_value.i = 1;
                    sf->operand_stack.push(&entry);
                } else if (*byte == 0x95) {
                    entry.context_value.i = -1;
                    sf->operand_stack.push(&entry);
                }
            }
        } break;
        case 0xb: // fconst_0
        case 0xc: // fconst_1
        case 0xd: // fconst_2
        {
            char e     = *byte - 0xb;
            auto entry = ContextEntry("", F, reinterpret_cast<void *>(&e));
            sf->operand_stack.push(&entry);
        } break;
        case 0x17: // fload
        case 0x15: // iload
        {
            auto index = *(byte + 1);
            byte++;
            auto value = sf->lva.at(index);
            sf->operand_stack.push(value);

        } break;
        case 0x22: // fload_0
        case 0x23: // fload_1
        case 0x24: // fload_2
        case 0x25: // fload_3
        {
            auto index = *(byte)-0x22;
            auto value = sf->lva.at(index);
            sf->operand_stack.push(value);

        } break;
        case 0x76: // fneg
        {
            auto value = *sf->operand_stack.top();
            sf->operand_stack.pop();
            float f = -1;
            auto result =
                value * ContextEntry("", F, reinterpret_cast<void *>(&f));
            sf->operand_stack.push(&result);

        } break;
        case 0x72: // frem
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            auto result =
                fmod(value1->context_value.f, value2->context_value.f);

            sf->operand_stack.push(
                &ContextEntry("", F, reinterpret_cast<void *>(&result)));

        } break;
        case 0x38: // fstore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index     = *(++byte);
            sf->lva[index] = value;

        } break;
        case 0x43: // fstore_0
        case 0x44: // fstore_1
        case 0x45: // fstore_2
        case 0x46: // fstore_3
        {
            auto index = *(byte)-0x43;
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            sf->lva[index] = value;

        } break;

        case 0xb4: // getfield
        {
            auto indexbyte1 = *(++byte);
            auto indexbyte2 = *(++byte);
            int index       = (indexbyte1 << 8) + indexbyte2;
            auto objref     = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (objref->isNull)
                throw std::runtime_error("NullPointerException");
            auto value = objref->cf->at(index);
            sf->operand_stack.push(value.get());
        } break;
        case 0xb2: // getstatic
        {
            // precisamos de teste pra fazer esse nao entendi 😬
        } break;
        case 0xa7: // goto
        {
            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);

            auto offset = (branchbyte1 << 8) | branchbyte2;

            byte += offset;

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
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = C;
            sf->operand_stack.push(value);
        } break;
        case 0x93: // i2s
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value->entry_type = S;
            sf->operand_stack.push(value);
        } break;
        case 0x7e: // iand
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto result = value1 & value2;
            sf->operand_stack.push(&result);
        } break;
        case 0x2: // iconst_m1
        case 0x3: // iconst_0
        case 0x4: // iconst_1
        case 0x5: // iconst_2
        case 0x6: // iconst_3
        case 0x7: // iconst_4
        case 0x8: // iconst_5
        {
            char e     = *byte - 0x3;
            auto entry = ContextEntry("", I, reinterpret_cast<void *>(&e));
            sf->operand_stack.push(&entry);

        } break;

        case 0x6c: // idiv
        {
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            if (value2.context_value.i == 0) {
                throw std::runtime_error("ArithmeticException");
            } else {
                auto value3 = value1 / value2;
                sf->operand_stack.push(&value3);
            }
        } break;

        case 0xa5: // if_acmpeq
        case 0xa6: // if_acmpne
        {
            auto index  = *(byte)-0xa5;
            auto value1 = *sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = *sf->operand_stack.top();
            sf->operand_stack.pop();

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
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            auto branchbyte1 = *(++byte);
            auto branchbyte2 = *(++byte);

            if (index == 0) {
                if (value1->context_value.i == value2->context_value.i) {
                    auto offset = (branchbyte1 << 8) | branchbyte2;
                    byte += offset;
                }
            } else if (index == 1) {
                if (value1->context_value.i != value2->context_value.i) {
                    auto offset = (branchbyte1 << 8) | branchbyte2;
                    byte += offset;
                } else if (index == 2) {
                    if (value1->context_value.i < value2->context_value.i) {
                        auto offset = (branchbyte1 << 8) | branchbyte2;
                        byte += offset;
                    } else if (index == 3) {
                        if (value1->context_value.i <=
                            value2->context_value.i) {
                            auto offset = (branchbyte1 << 8) | branchbyte2;
                            byte += offset;
                        } else if (index == 4) {
                            if (value1->context_value.i >
                                value2->context_value.i) {
                                auto offset = (branchbyte1 << 8) | branchbyte2;
                                byte += offset;
                            } else if (index == 5) {
                                if (value1->context_value.i >=
                                    value2->context_value.i) {
                                    auto offset =
                                        (branchbyte1 << 8) | branchbyte2;
                                    byte += offset;
                                }
                            }
                        }
                    }
                }
            }
        } break;

        case 0x99: // ifeq
        case 0x9a: // ifne
        case 0x9b: // iflt
        case 0x9c: // ifge
        case 0x9d: // ifgt
        case 0x9e: // ifle
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            int n = *(byte)-0x99;
            if (n == 0) {
                if (!value->context_value.i) {
                    auto branchbyte1 = *(++byte);
                    auto branchbyte2 = *(++byte);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                    byte             = byte + offset;
                } else if (n == 1) {
                    if (value->context_value.i) {
                        auto branchbyte1 = *(++byte);
                        auto branchbyte2 = *(++byte);
                        auto offset      = (branchbyte1 << 8) | branchbyte2;
                        byte             = byte + offset;
                    }
                } else if (n == 2) {
                    if (value->context_value.i < 0) {
                        auto branchbyte1 = *(++byte);
                        auto branchbyte2 = *(++byte);
                        auto offset      = (branchbyte1 << 8) | branchbyte2;
                        byte             = byte + offset;
                    }
                } else if (n == 3) {
                    if (value->context_value.i >= 0) {
                        auto branchbyte1 = *(++byte);
                        auto branchbyte2 = *(++byte);
                        auto offset      = (branchbyte1 << 8) | branchbyte2;
                        byte             = byte + offset;
                    }
                } else if (n == 4) {
                    if (value->context_value.i > 0) {
                        auto branchbyte1 = *(++byte);
                        auto branchbyte2 = *(++byte);
                        auto offset      = (branchbyte1 << 8) | branchbyte2;
                        byte             = byte + offset;
                    }
                } else {
                    if (value->context_value.i <= 0) {
                        auto branchbyte1 = *(++byte);
                        auto branchbyte2 = *(++byte);
                        auto offset      = (branchbyte1 << 8) | branchbyte2;
                        byte             = byte + offset;
                    }
                }
            }
        } break;
        case 0xc6: // ifnull
        case 0xc7: // ifnonnull
        {
            auto index = *(byte)-0xc6;
            auto value = *sf->operand_stack.top();
            sf->operand_stack.pop();
            if (index == 0) {
                if (value.isNull) {
                    auto branchbyte1 = *(++byte);
                    auto branchbyte2 = *(++byte);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                }
            } else {
                if (!value.isNull) {
                    auto branchbyte1 = *(++byte);
                    auto branchbyte2 = *(++byte);
                    auto offset      = (branchbyte1 << 8) | branchbyte2;
                }
            }
        } break;
        case 0x84: // iinc
        {
            //  char constant;
            //  auto value = sf->operand_stack.top();
            //     sf->operand_stack.pop();
            //     auto index         = *(++byte);
            //     sf->lva[index]     = value;

        } break;
        case 0xc1: // instanceof
        {
            // I'm not sure about if this will work or not;
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto objref        = sf->operand_stack.top();
            int zero           = 0;
            int one            = 1;
            sf->operand_stack.pop();
            if (objref->isNull) {
                // push 0 into the stack
                sf->operand_stack.push(
                    &ContextEntry("", I, reinterpret_cast<void *>(&zero)));
            } else {
                if (objref->class_name == cp->getNameByIndex(index)) {
                    sf->operand_stack.push(
                        &ContextEntry("", I, reinterpret_cast<void *>(&one)));
                } else {
                    sf->operand_stack.push(
                        &ContextEntry("", I, reinterpret_cast<void *>(&zero)));
                }
            }
        } break;
        case 0x1a: // iload_0
        case 0x1b: // iload_1
        case 0x1c: // iload_2
        case 0x1d: // iload_3
        {
            auto index = *(byte)-0x26;
            auto value = sf->lva.at(index);
            sf->operand_stack.push(value);
            if (!value->context_value.i) {
                auto branchbyte1 = *(++byte);
                auto branchbyte2 = *(++byte);
                auto offset      = (branchbyte1 << 8) | branchbyte2;
                byte             = byte + offset;
            }
        } break;
        case 0xb7: // invokespecial
        {
            auto indexbyte1    = *(++byte);
            auto indexbyte2    = *(++byte);
            unsigned int index = (indexbyte1 << 8) + indexbyte2;
            auto code          = cm[index].attributes[0].code;
            Exec(code, sf->lva);
        } break;
        default:
            break;
        }
    }
    std::cout << "operand stack" << std::endl;
    while (not sf->operand_stack.empty()) {
        std::cout << sf->operand_stack.top()->class_name << " ";
        sf->operand_stack.top()->PrintValue();
        std::cout << std::endl;
        sf->operand_stack.pop();
    }
}