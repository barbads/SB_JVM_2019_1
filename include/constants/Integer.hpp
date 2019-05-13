struct Integer {
    unsigned short int tag = 3;
    // values represent int constant value, big-endian
    int value;
    Integer(int val) : value(val){};
};
