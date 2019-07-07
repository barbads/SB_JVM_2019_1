/**
 * Represents the constant pool entry of the type CONSTANT_Float_info (tag = 4)
 * more information at:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.4.4
 */
struct Float {
    unsigned short int tag = 4;
    float value;
    Float(float val) : value(val){};
};
