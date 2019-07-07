/**
 * Represents the constant pool entry of the type CONSTANT_Integer_info (tag =
 * 3) more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.4
 */
struct Integer {
    unsigned short int tag = 3;
    int value; /// value represents int constant value, big-endian
    Integer(int val) : value(val){};
};
