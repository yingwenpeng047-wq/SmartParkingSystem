#ifndef CTOOL_H
#define CTOOL_H


class CTool
{
public:
    CTool();
    static unsigned int crc_table[256];
    static void init_crc_table(void);
    static unsigned int crc32(unsigned int crc, unsigned char* buffer, unsigned int size);
};


#endif // CTOOL_H
