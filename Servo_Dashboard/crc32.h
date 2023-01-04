//! Class for generating STM32 compatible 32bit CRC

#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

class Crc32
{
    public:
        uint32_t get(uint32_t *data, uint32_t length);
};

#endif // CRC32_H
