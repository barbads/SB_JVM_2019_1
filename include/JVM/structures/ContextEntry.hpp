#ifndef _ContextEntry_H_
#define _ContextEntry_H_

#include <JVM/structures/Types.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ContextEntry {
  private:
    bool hasContext;
    std::string field_name;
    std::string string_instance;
    bool isRetAddr = false;

  public:
    std::map<std::string, std::shared_ptr<ContextEntry>> *cf;
    std::vector<std::shared_ptr<ContextEntry>> arrayRef;
    bool isNull;
    Type entry_type;
    std::string class_name;
    std::vector<std::shared_ptr<ContextEntry>> l;

    ~ContextEntry() {}

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

    ContextEntry(std::string className, Type entryType, void *value) {
        l                = std::vector<std::shared_ptr<ContextEntry>>();
        this->class_name = className;
        this->entry_type = entryType;
        isArray          = false;
        hasContext       = false;
        isNull           = false;
        switch (entryType) {
        case B:
            context_value.i = *((short int *)(value));
            context_value.b = *reinterpret_cast<unsigned char *>(value);
            break;
        case I:
            context_value.i = *((int *)(value));
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
                    (std::vector<std::shared_ptr<ContextEntry>> *)(value);
                for (auto c : *received_context) {
                    l.push_back(c);
                }
            } else {
                isNull = true;
            }
            break;
        case R:
            class_name      = "Ljava/lang/String";
            string_instance = *reinterpret_cast<std::string *>(value);
            break;
        default:
            throw std::runtime_error("This type is not recognized");
            break;
        }
    }

    ContextEntry(Type entryType, int arraySize) {
        entry_type = entryType;
        isArray    = true;
        hasContext = false;
        isNull     = false;
        l          = std::vector<std::shared_ptr<ContextEntry>>();
        arrayRef   = std::vector<std::shared_ptr<ContextEntry>>(arraySize);
        for (auto &ref : arrayRef) {
            int zero = 0;
            if (entryType != L) {
                ref = std::make_shared<ContextEntry>(
                    "", entryType, reinterpret_cast<void *>(&zero));
            } else {
                ref = std::make_shared<ContextEntry>();
            }
        }
    }

    ContextEntry() {
        isNull          = true;
        l               = std::vector<std::shared_ptr<ContextEntry>>();
        context_value.i = 0;
    }

    ContextEntry(std::map<std::string, std::shared_ptr<ContextEntry>> *cf,
                 void *value) {
        // case an objectref has fields
        isNull     = false;
        hasContext = true;
        isArray    = false;
        entry_type = L;
        if (cf != nullptr) {
            this->cf = cf;
        }
        l = std::vector<std::shared_ptr<ContextEntry>>();
        if (value != nullptr) {
            hasContext = true;
            auto received_context =
                (std::vector<std::shared_ptr<ContextEntry>> *)(value);
            for (auto c : *received_context) {
                l.push_back(c);
            }
        }
    }

    void PrintValue() {
        if (hasContext) {
            for (auto entry : l) {
                std::cout << "\n\tClass Name " << class_name << "\n\tValue:";
                entry->PrintValue();
                std::cout << std::endl;
            }
        } else {
            if (class_name == "Ljava/lang/String") {
                std::cout << string_instance;
                return;
            }
        }
        switch (entry_type) {
        case B:
            std::cout << (unsigned int)context_value.b;
            break;
        case I:
            std::cout << context_value.i;
            break;
        case D:
            std::cout << context_value.d;
            break;
        case F:
            std::cout << std::fixed << std::setprecision(2)
                      << (float)context_value.f;
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

    void setAsRetAddress() { isRetAddr = true; }

    bool isReturnAddress() { return isRetAddr; }

    std::vector<std::shared_ptr<ContextEntry>> *getArray() {
        if (!isNull) {
            return &arrayRef;
        }
        throw std::runtime_error("NullPointerException");
    }

    void addToArray(int index, std::shared_ptr<ContextEntry> ce) {
        if (!isArray) {
            throw std::runtime_error("Could not push to a not-array structure");
        }
        if (ce->entry_type != this->entry_type) {
            throw std::runtime_error(
                "ArrayIndexOutOfBoundsException: Could not add a reference "
                "from a different type into array");
        }
        if (index >= arrayRef.size()) {
            throw std::runtime_error("ArrayIndexOutOfBoundsException");
        }
        auto pos = arrayRef.begin() + index;
        arrayRef.insert(pos, ce);
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
        return ContextEntry();
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
        return ContextEntry();
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
        return ContextEntry();
    }

    ContextEntry operator||(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b || b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i || b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j || b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s || b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
        return ContextEntry();
    }

    ContextEntry operator^(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            auto nvalue = context_value.b ^ b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            auto nvalue = context_value.i ^ b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            auto nvalue = context_value.j ^ b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            auto nvalue = context_value.s ^ b.context_value.s;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        default:
            break;
        }
        return ContextEntry();
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
        return ContextEntry();
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
        return ContextEntry();
    }

    bool operator==(const ContextEntry b) const {
        switch (entry_type) {
        case B: {
            return context_value.b == b.context_value.b;
        } break;
        case I: {
            return context_value.i == b.context_value.i;
        } break;
        case D: {
            return context_value.d == b.context_value.d;
        } break;
        case F: {
            return context_value.f == b.context_value.f;
        } break;
        case J: {
            return context_value.j == b.context_value.j;
        } break;
        case S: {
            return context_value.s == b.context_value.s;
        } break;
        default:
            if (isArray) {
                if (arrayRef.size() != b.arrayRef.size())
                    return false;
                return (arrayRef == b.arrayRef);
            } else if (l.size() > 0) {
                if (l.size() != b.l.size())
                    return false;
                return l == b.l;
            }
            break;
        }
        return false;
    }
};

#endif