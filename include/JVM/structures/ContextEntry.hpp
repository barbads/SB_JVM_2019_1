#ifndef _ContextEntry_H_
#define _ContextEntry_H_

#include <JVM/structures/Types.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ContextEntry {
  private:
    bool hasContext;
    bool isNull;
    std::vector<ContextEntry> l;
    std::vector<std::shared_ptr<ContextEntry>> arrayRef;
    std::string fieldName;

  public:
    Type entry_type;
    std::string class_name;
    bool isArray;
    union ContextEntryUnion {
        unsigned char b;
        int i;
        char c;
        double d;
        float f;
        long j;
        short s;
        bool z;
    } context_value;

    ContextEntry operator+(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b + b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i + b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case D: {
            auto nvalue = context_value.d + b.context_value.d;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case F: {
            auto nvalue = context_value.f + b.context_value.f;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j + b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s + b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
    }

    ContextEntry operator/(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b / b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i / b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case D: {
            auto nvalue = context_value.d / b.context_value.d;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case F: {
            auto nvalue = context_value.f / b.context_value.f;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j / b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s / b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
    }

    ContextEntry operator&(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b & b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i & b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case D: {
            auto nvalue = context_value.d & b.context_value.d;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case F: {
            auto nvalue = context_value.f & b.context_value.f;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j & b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s & b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
    }

    ContextEntry operator*(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b * b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i * b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case D: {
            auto nvalue = context_value.d * b.context_value.d;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case F: {
            auto nvalue = context_value.f * b.context_value.f;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j * b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s * b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
    }

    

    ContextEntry operator-(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b - b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i - b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case D: {
            auto nvalue = context_value.d - b.context_value.d;
            if (b.context_value.d == 0.0 && context_value.d == 0.0) {
                nvalue = 0.0;
            }
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case F: {
            auto nvalue = context_value.f - b.context_value.f;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j - b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s - b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
    }

    ContextEntry(std::string className, std::string fieldName, Type entryType, void *value) {
        this->class_name = className;
        this->field_name = fieldName;
        this->entry_type = entryType;
        isArray          = false;
        hasContext       = false;
        isNull           = false;
        switch (entryType) {
        case B:
            context_value.b = *reinterpret_cast<unsigned char *>(value);
            break;
        case I:
            context_value.i = *reinterpret_cast<int *>(value);
            break;
        case D:
            context_value.d = *reinterpret_cast<double *>(value);
            break;
        case F:
            context_value.f = *reinterpret_cast<float *>(value);
            break;
        case J:
            context_value.j = *reinterpret_cast<long *>(value);
            break;
        case S:
            context_value.s = *reinterpret_cast<short *>(value);
            break;
        case L:
            if (value != nullptr) {
                hasContext = true;
                isArray    = false;
                auto received_context =
                    reinterpret_cast<std::vector<ContextEntry> *>(value);
                for (auto c : *received_context) {
                    l.push_back(c);
                }
            } else {
                isNull = true;
            }
            break;
        default:
            throw std::runtime_error("This type is not recognized");
            break;
        }
    }

    ContextEntry(Type entryType, int arraySize) {
        entry_type = entryType;
        arrayRef   = std::vector<std::shared_ptr<ContextEntry>>(arraySize);
        for (auto &ref : arrayRef) {
            ref = nullptr;
        }
    }

    ContextEntry() { isNull = true; }

    void PrintValue() {
        if (hasContext) {
            for (auto entry : l) {
                std::cout << "\n\tClass Name " << class_name << "\n\tValue:";
                entry.PrintValue();
                std::cout << std::endl;
            }
        }
        switch (entry_type) {
        case B:
            std::cout << context_value.b;
            break;
        case I:
            std::cout << context_value.i;
            break;
        case D:
            std::cout << context_value.d;
            break;
        case F:
            std::cout << context_value.f;
            break;
        case J:
            std::cout << context_value.j;
            break;
        case S:
            std::cout << context_value.s;
            break;
        default:
            break;
        }
    }

    bool isReference() {
        // Case hascontext so its a class entry
        if (hasContext || isArray)
            return true;
        // Case array we need to check if all elements are of type ref
        return true;
    }

    std::vector<ContextEntry> getArray() {
        if (!isNull) {
            auto array = std::vector<ContextEntry>(arrayRef.size());
            for (auto i = 0; i < arrayRef.size(); i++) {
                if (arrayRef[i] != nullptr) {
                    array[i] = *arrayRef[i];
                }
            }
            return array;
        }
        throw std::runtime_error("NullPointerException");
    }

    void addToArray(int index, ContextEntry ce) {
        if (!isArray) {
            throw std::runtime_error("Could not push to a not-array structure");
        }
        if (ce.entry_type != this->entry_type) {
            throw std::runtime_error(
                "ArrayIndexOutOfBoundsException: Could not add a reference "
                "from a different type into array");
        }
        if (index >= arrayRef.size()) {
            throw std::runtime_error("ArrayIndexOutOfBoundsException");
        }
        auto pos = arrayRef.begin() + index;
        arrayRef.insert(pos, std::make_shared<ContextEntry>(ce));
    }

    ContextEntry arrayLength() {
        if (!isArray) {
            throw std::runtime_error(
                "Could not count length in a non-array structure");
        }
        if (isNull) {
            throw std::runtime_error("NullPointerException");
        }
        int length = arrayRef.size();
        return ContextEntry("", I, reinterpret_cast<void *>(&length));
    }
};

#endif