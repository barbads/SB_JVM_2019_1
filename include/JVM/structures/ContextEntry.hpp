#ifndef _ContextEntry_H_
#define _ContextEntry_H_

#include <JVM/structures/Types.hpp>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * ContextEntry defines any operand/variable in code execution, it has control
 * flags to especaial cases such as objects and arrays, its data is saved in a
 * union unless it is a object value/ref in those cases, class_fields (cf) or
 * array ref are set
 */
class ContextEntry {
  private:
    bool hasContext;
    std::string field_name;
    bool isRetAddr = false;

  public:
    std::string string_instance;
    std::map<std::string, std::shared_ptr<ContextEntry>> cf;
    std::vector<std::shared_ptr<ContextEntry>> arrayRef;
    bool isNull;
    Type entry_type;
    std::string class_name;
    std::vector<std::shared_ptr<ContextEntry>> l;

    ~ContextEntry() {}

    ///
    /// Controll flag to identify if it is an array
    ///
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

    ///
    /// Constructor to base case: takes a string name a entryType and a void *
    /// value based on entry type it cast void * pointer to proper pointer and
    /// save the value inside union's correct field
    ///
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
            context_value.j = *((long *)(value));
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
            class_name      = "java/lang/String";
            string_instance = *reinterpret_cast<std::string *>(value);
            break;
        case C:
            context_value.c = *reinterpret_cast<unsigned char *>(value);
            break;
        default:
            throw std::runtime_error("This type is not recognized");
            break;
        }
    }

    ///
    /// Constructor for array of basic types or null references - case you need
    /// only to create an array of size with null elements
    ///
    ContextEntry(std::string class_name, Type entryType, int arraySize) {
        this->class_name = class_name;
        entry_type       = entryType;
        isArray          = true;
        hasContext       = false;
        isNull           = false;
        l                = std::vector<std::shared_ptr<ContextEntry>>();
        arrayRef = std::vector<std::shared_ptr<ContextEntry>>(arraySize);
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

    ///
    /// Creates an array with references to other objects, in this case each
    /// object will have fields for that class instance
    ///
    ContextEntry(std::string class_name, Type entryType, int arraySize,
                 std::map<std::string, std::shared_ptr<ContextEntry>> cf) {
        if (entryType != L) {
            throw std::runtime_error("Could not construct a ContextEntry array "
                                     "of objects if type is different from L");
        }
        this->class_name = class_name;
        entry_type       = entryType;
        isArray          = true;
        hasContext       = false;
        isNull           = false;
        l                = std::vector<std::shared_ptr<ContextEntry>>();
        arrayRef = std::vector<std::shared_ptr<ContextEntry>>(arraySize);
        for (auto &ref : arrayRef) {
            int zero = 0;
            ref      = std::make_shared<ContextEntry>();
            ref->cf  = cf;
        }
    }

    ///
    /// Create a null entry
    ///
    ContextEntry() {
        isNull          = true;
        l               = std::vector<std::shared_ptr<ContextEntry>>();
        context_value.i = 0;
        entry_type      = L;
    }

    ///
    /// Create a not-null object reference containing its fields;
    ///
    ContextEntry(std::map<std::string, std::shared_ptr<ContextEntry>> cf,
                 std::string class_name) {
        // case an objectref has fields
        isNull           = false;
        this->class_name = class_name;
        hasContext       = true;
        isArray          = false;
        entry_type       = L;
        this->cf         = cf;
    }

    ///
    /// Print value handles print for it's internal value, calling cout for
    /// saved value, controled by entry_type
    ///
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
            std::cout << context_value.f;
            break;
        case J:
            std::cout << (long)context_value.j;
            break;
        case S:
            std::cout << context_value.s;
            break;
        case R:
            std::cout << string_instance;
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

    ///
    /// set context entry as a return address
    ///
    void setAsRetAddress() { isRetAddr = true; }

    ///
    /// returns if value inside context_entry is a return addres
    ///
    bool isReturnAddress() { return isRetAddr; }

    ///
    /// for array instances returns a pointer to saved array internal
    ///
    std::vector<std::shared_ptr<ContextEntry>> *getArray() {
        if (!isNull) {
            return &arrayRef;
        }
        throw std::runtime_error("NullPointerException");
    }

    ///
    /// Push an context_entry into an internal array
    ///
    void addToArray(int index, std::shared_ptr<ContextEntry> ce) {
        if (!isArray) {
            throw std::runtime_error("Could not push to a not-array structure");
        }
        if (ce->entry_type != this->entry_type && ce->entry_type != R) {
            throw std::runtime_error(
                "ArrayIndexOutOfBoundsException: Could not add a reference "
                "from a different type into array");
        }
        if (index > arrayRef.size()) {
            throw std::runtime_error("ArrayIndexOutOfBoundsException");
        } else if (index == arrayRef.size()) {
            arrayRef.push_back(ce);
        } else {
            auto pos = arrayRef.begin() + index;
            arrayRef.insert(pos, ce);
        }
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
            unsigned nvalue = context_value.b & b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            int nvalue = context_value.i & b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            long nvalue = context_value.j & b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            short nvalue = context_value.s & b.context_value.s;
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
            unsigned char nvalue = context_value.b || b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            int nvalue = context_value.i || b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            long nvalue = context_value.j || b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            short nvalue = context_value.s || b.context_value.s;
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
            unsigned char nvalue = context_value.b ^ b.context_value.b;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case I: {
            int nvalue = context_value.i ^ b.context_value.i;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case J: {
            long nvalue = context_value.j ^ b.context_value.j;
            return ContextEntry("", entry_type,
                                reinterpret_cast<void *>(&nvalue));
        } break;
        case S: {
            short nvalue = context_value.s ^ b.context_value.s;
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