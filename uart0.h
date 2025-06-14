// UART0 Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef UART0_H_
#define UART0_H_

#include <stdint.h>
#include <stdbool.h>


#define MAX_CHARS 80
#define MAX_FIELDS 5

typedef struct _USER_DATA
{
  char buffer[MAX_CHARS+1]; //stores the user input. Its size is 80. The +1 is for the null terminator
  uint8_t fieldCount;       //Keeps track of how many fields are found in the word that the user typed in
  uint8_t fieldPosition[MAX_FIELDS]; //Stores the starting index of each field in the buffer
  char fieldType[MAX_FIELDS]; //Stores the type of each field. Like 'a' for alpha, 'n' for numeric
} USER_DATA;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void initUart0();
void setUart0BaudRate(uint32_t baudRate, uint32_t fcyc);
void putcUart0(char c);
void putsUart0(char* str);
char getcUart0();
bool kbhitUart0();
char* getsUart0(USER_DATA *data);
void parseFields(USER_DATA *data);
char* getFieldString(USER_DATA *data, uint8_t fieldNumber);
int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber);
bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments);
int compare_strings(const char *str1, const char *str2);


#endif
