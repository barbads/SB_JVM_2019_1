struct UTF8 {
    unsigned short int tag = 1;
    unsigned short int lenght;
    unsigned char *bytes;
    UTF8(unsigned short int lenght) {
        this->lenght = lenght;
        bytes        = new unsigned char[lenght];
    }
};
