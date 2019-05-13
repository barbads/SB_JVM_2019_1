
struct InterfaceMethodref {
    unsigned short int tag = 11;
    int class_index;
    int name_type_index;
    InterfaceMethodref(int cidx, int ntidx)
        : class_index(cidx), name_type_index(ntidx){};
};
