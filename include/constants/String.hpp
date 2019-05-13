
struct String {
    unsigned short int tag = 8;
    int string_index;
    String(int sidx) : string_index(sidx){};
};
