#include <MethodExecuter/MethodExecuter.hpp>
#include <math.h>

MethodExecuter::MethodExecuter(std::vector<unsigned char> code,
                               ConstantPool *cp) {
    bytecode = code;
    this->cp = cp;
}

ContextEntry MethodExecuter::Exec(std::vector<ContextEntry> ce) {
    sf = new StackFrame(ce);
    std::vector<int> args;
    int args_counter = 0;
    bool wide        = false;
    for (auto byte = bytecode.begin(); byte != bytecode.end(); byte++) {
        switch (*byte) {
        case 0x32: // aaload
        {
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top().isArray) {
                auto arrayRef = sf->operand_stack.top().getArray();
                sf->operand_stack.pop();
                if (index >= arrayRef.size()) {
                    throw std::runtime_error("ArrayIndexOutOfBoundsException");
                }
                sf->operand_stack.push(arrayRef.at(index));
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x53: // aastore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            if (sf->operand_stack.top().isArray) {
                sf->operand_stack.top().addToArray(index, value);
            } else {
                throw std::runtime_error(
                    "Stack operand is not an array reference");
            }
        } break;
        case 0x01: // aconst_null
        {
            const auto nil = ContextEntry();
            sf->operand_stack.push(nil);
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
            if (load.isReference()) {
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
            if (load.isReference()) {
                sf->operand_stack.push(load);
            } else {
                throw std::runtime_error("aload of non-reference object");
            }
        } break;
        case 0xbd: // anewarray
        {
            int count = sf->operand_stack.top().context_value.i;
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
            sf->operand_stack.push(ContextEntry(L, count));
        } break;
        case 0xb0: // areturn
        {
            auto retval = sf->operand_stack.top();
            while (!sf->operand_stack.empty()) {
                sf->operand_stack.pop();
            }
            if (!retval.isReference())
                throw std::runtime_error(
                    "areturn cannot return a non-reference value");
            return retval;
        } break;
        case 0xbe: // arraylength
        {
            auto arrRef = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (!arrRef.isReference()) {
                std::runtime_error(
                    "arraylength called over a non-reference object");
            }

            auto length = arrRef.arrayLength();
            sf->operand_stack.push(length);
        } break;
        case 0x3a: // astore
        {
            int index = *(byte + 1);
            byte++;
            auto objRef = sf->operand_stack.top();
            sf->operand_stack.pop();
            if (!objRef.isReference()) {
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
            if (!objRef.isReference()) {
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
            sf->operand_stack.push(entry);
            byte += 2;
        } break;
        case 0x59: // dup
        {
            auto top = sf->operand_stack.top();
            sf->operand_stack.push(top);
        } break;
        //// comecei aqui
        case 0x33: // baload
        case 0x34: // caload
        case 0x31: // daload
        {
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            auto arrayref = sf->operand_stack.top().getArray();
            sf->operand_stack.pop();

            if (arrayref.size() >= index)
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            auto value = arrayref.at(index);
            sf->operand_stack.push(value);

        } break;

        case 0x54: // bastore
        case 0x55: // castore
        case 0x52: // dastore
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto index = sf->operand_stack.top().context_value.i;
            sf->operand_stack.pop();
            auto arrayref = sf->operand_stack.top().getArray();
            sf->operand_stack.pop();

            arrayref[index] = value;
            if (arrayref.size() >= index)
                throw std::runtime_error("ArrayIndexOutOfBoundsException");
            auto value = arrayref.at(index);
            sf->operand_stack.push(value);

        } break;
        case 0x10: // bipush
        {
            auto value = *(byte + 1);
            byte++;
            auto entry = ContextEntry("", B, reinterpret_cast<void *>(&value));
            sf->operand_stack.push(entry);

        } break;

        case 0xc0: // checkcast
                   // Vieiraaaa salva nois

        case 0x90: // d2f
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value.entry_type = F;
            sf->operand_stack.push(value);

        } break;
        case 0x8e: // d2i
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value.entry_type = I;
            sf->operand_stack.push(value);

        } break;
        case 0x8f: // d2l
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            value.entry_type = L;
            sf->operand_stack.push(value);

        } break;

        case 0x63: // dadd
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            sf->operand_stack.push(value1 + value2);

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
            if (value1.context_value.d > value2.context_value.d) {
                entry.context_value.i = 1;
                sf->operand_stack.push(entry);
            } else if (value1.context_value.d < value2.context_value.d) {
                entry.context_value.i = -1;
                sf->operand_stack.push(entry);
            } else if (value1.context_value.d == value2.context_value.d) {
                entry.context_value.i = 0;
                sf->operand_stack.push(entry);
            } else if (isnan(value1.context_value.d) ||
                       isnan(value2.context_value.d)) {
                if (*byte == 0x98) {
                    entry.context_value.i = 1;
                    sf->operand_stack.push(entry);
                } else if (*byte == 0x97) {
                    entry.context_value.i = -1;
                    sf->operand_stack.push(entry);
                }
            }
        } break;
        case 0xe: // dconst_<d> dconst_0
        case 0xf: // dconst_<d> dconst_1
        {
            char e     = *byte - 0xe;
            auto entry = ContextEntry("", D, reinterpret_cast<void *>(&e));
            sf->operand_stack.push(entry);

        } break;
        case 0x6f: // ddiv
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            sf->operand_stack.push(value1 / value2);

        } break;
        case 0x18: // dload
        {
            auto index = *(byte + 1);
            byte++;
            auto value = sf->lva.at(index);
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

        } break;

        case 0x6b: // dmul
        {
            auto value1 = sf->operand_stack.top();
            sf->operand_stack.pop();
            auto value2 = sf->operand_stack.top();
            sf->operand_stack.pop();

            sf->operand_stack.push(value1 * value2);
        }
        case 0x77: // dneg
        {
            auto value = sf->operand_stack.top();
            sf->operand_stack.pop();
            double d = -1;
            sf->operand_stack.push(
                value * ContextEntry("", D, reinterpret_cast<void *>(&d)));

        } break;
        case 0x73: // drem
        case 0xaf: // dreturn

        default:
            break;
        }
    }
    std::cout << "operand stack" << std::endl;
    while (not sf->operand_stack.empty()) {
        std::cout << sf->operand_stack.top().class_name << " ";
        sf->operand_stack.top().PrintValue();
        std::cout << std::endl;
        sf->operand_stack.pop();
    }
}
