#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

char* toAsciiHex(char* buff, uint32_t Val)
{
    int i=7;
    int temp;
    for (i = 7; i >= 0; i--)
    {
            //uint8_t hexDigit = (Val >> (28 - (i * 4))) & 0xF;
            //buff[i] = (hexDigit < 10) ? (hexDigit + '0') : (hexDigit - 10 + 'a');
                temp = Val % 16;

                if (temp < 10)
                {
                    temp = temp + 48; //'0'= 48
                    buff[i] = temp;
                }
                else
                {
                    temp = temp + 55; //55 = 'F'
                    buff[i] = temp;
                }
                Val = Val / 16;
            }

    buff[8] = '\0';
    return buff;
}
