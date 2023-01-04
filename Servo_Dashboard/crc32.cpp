#include "crc32.h"

/*!
    Returns CRC for "*data" with given length
*/
uint32_t Crc32::get(uint32_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    uint32_t b = 0;
    uint32_t i = 0;

    while(b < length)
    {
        i = 0;
        crc = crc ^ data[b];
        while(i < 32)
        {
            if(crc & 0x80000000)
            {
                crc = (crc << 1) ^ 0x04C11DB7; //Polynomial used in STM32
            }
            else
            {
                crc = (crc << 1);
            }
            i++;
        }
        b++;
    }

    return crc;
}
