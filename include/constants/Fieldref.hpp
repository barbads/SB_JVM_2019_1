
struct Fieldref {
    unsigned short int tag = 9;
    unsigned short int class_index;
    // class_index is a valid index to constant_pool table
    // represents class's full name or interface
    // containing this field declaration
    unsigned short int name_type_index;
    // name_type_index is a valid index to constant_pool
    // cte name and type indicating field's name and descriptor
    Fieldref(int classidx, int ntidx)
        : class_index(classidx), name_type_index(ntidx){};
};
