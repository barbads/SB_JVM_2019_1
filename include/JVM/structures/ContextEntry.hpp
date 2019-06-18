#ifndef _ContextEntry_H_
#define _ContextEntry_H_

#include <JVM/structures/Types.hpp>
#include <iostream>
#include <string>
#include <vector>

struct ContextEntry {
    Type entry_type;
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
    bool hasContext;
    std::vector<ContextEntry> l;
    bool isArray;
    std::vector<ContextEntry> arrayRef;
    std::string fieldName;
    std::string class_name;
    ContextEntry(std::string className, Type entryType, void *value) {
        this->class_name = className;
        this->entry_type = entryType;
        isArray          = false;
        hasContext       = false;
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
            }
            break;
        case A:
            if (value != nullptr) {
                isArray    = true;
                hasContext = false;
                auto received_array =
                    reinterpret_cast<std::vector<ContextEntry> *>(value);
                for (auto a : *received_array) {
                    arrayRef.push_back(a);
                }
            }
        default:
            throw std::runtime_error("This type is not recognized");
            break;
        }
    }
    ContextEntry() {}
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

    bool IsReference() {
        // Case hascontext so its a class entry
        if (hasContext)
            return true;
        // Case array we need to check if all elements are of type ref
        if (isArray)
            for (auto elem : arrayRef) {
                if (!elem.IsReference())
                    return false;
            }
        return true;
    }

    void AddToArray(int index, ContextEntry ce) {
        if (!isArray) {
            throw std::runtime_error("Could not push to a not-array structure");
        }
        if (ce.entry_type != this->entry_type) {
            throw std::runtime_error(
                "Could not add a reference from a different type into array");
        }
        auto pos = arrayRef.begin() + index;
        arrayRef.insert(pos, ce);
    }
};

#endif