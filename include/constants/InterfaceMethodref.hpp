
struct InterfaceMethodref {
    unsigned short int tag = 11;
    // class_index is a valid index to constant_pool
    int class_index;
    // name_and_type_index is a valid index to constant_pool
    int name_type_index;
    InterfaceMethodref(int cidx, int ntidx)
        : class_index(cidx), name_type_index(ntidx){};
};
