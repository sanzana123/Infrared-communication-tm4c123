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

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "debug.h"
#include <string.h>

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

#define MAX_CHARS 80
#define MAX_FIELDS 5

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//typedef struct _USER_DATA
//{
//  char buffer[MAX_CHARS+1]; //stores the user input. Its size is 80. The +1 is for the null terminator
//  uint8_t fieldCount;       //Keeps track of how many fields are found in the word that the user typed in
//  uint8_t fieldPosition[MAX_FIELDS]; //Stores the starting index of each field in the buffer
//  char fieldType[MAX_FIELDS]; //Stores the type of each field. Like 'a' for alpha, 'n' for numeric
//} USER_DATA;

// Initialize UART0
void initUart0()
{
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);

    // Configure UART0 pins
    GPIO_PORTA_DR2R_R |= UART_TX_MASK;                  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;    // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
                                                        // select UART0 to drive pins PA0 and PA1: default, added for clarity

    // Configure UART0 to 115200 baud, 8N1 format
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R = 45;                                  // round(fract(r)*64)=45
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // enable TX, RX, and module
}

// Set baud rate as function of instruction cycle frequency
void setUart0BaudRate(uint32_t baudRate, uint32_t fcyc)
{
    uint32_t divisorTimes128 = (fcyc * 8) / baudRate;   // calculate divisor (r) in units of 1/128,
                                                        // where r = fcyc / 16 * baudRate
    divisorTimes128 += 1;                               // add 1/128 to allow rounding
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_IBRD_R = divisorTimes128 >> 7;                // set integer value to floor(r)
    UART0_FBRD_R = ((divisorTimes128) >> 1) & 63;       // set fractional value to round(fract(r)*64)
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // turn-on UART0
}

// Blocking function that writes a serial character when the UART buffer is not full
// write character to fifo,
//UART0_DR_R = UART0 data register.
//That data register is used to send characters over UART0.
//Writing to this register places the character over the transmit fifo
//From where it is transmitted serially over the transmit fifo

//UART_FR_TXFF = Bit mask for the Fifo transmit function. When bit is set to 1, it means that the fifo is full
//If bit is set to 1(fifo is full), the loop continues to rotate rotate to stop any further execution


void putcUart0(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
        putcUart0(str[i++]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{

    // FIFO overflow detected: UART_FR_RXOV = checks for overflow
    char hex_buffer[9];
    int i;
    int j;
    if (UART0_FR_R & UART_FR_RXFF) {
        unsigned int status_word = UART0_FR_R;  // Capture the status word

        const char *message = "FIFO FULL";
        for (i = 0; message[i] != '\0'; i++) {
             putcUart0(message[i]);  // Send "FIFO FULL 0x" to UART0
        }

        toAsciiHex(hex_buffer, status_word);

        for (j = 0; hex_buffer[j] != '\0'; j++) {
             putcUart0(hex_buffer[j]);  // Send the hexadecimal status word to UART0
        }
        putcUart0('\n');  // Send a newline character for readability

    }

    //Check if the FIFO has overflowed
    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;                        // get character from fifo
}

// Returns the status of the receive buffer
bool kbhitUart0()
{
    return !(UART0_FR_R & UART_FR_RXFE);
}

char* getsUart0(USER_DATA *data)
{
    //if count of characters > 0, decrement count to remove backspace characters
    /*int count_chars = 0;

    character = getcUart0();
    //If the character received is not a carriage return or a new line of the count of characters is stll
    //less than the maximum number of characters
        while ((character != '\r') && (character !='\n') && (count_chars < MAX_CHARS))

            //If character is a backspace, decrement the count of characters
            if (character==8)
            {
                if (count_chars>0)
                {
                    count_chars--;
                }
            }
            //For all printable characters, increment the count of characters and add the character to the buffer
            else
            {
                data->buffer[count_chars++] = c;
            }
        //If character is new line of a carriage return, add a null terminator
        data->buffer[count_chars] = '\0';*/

        int count =0;
            char character;

            while(1)
            {
                character = getcUart0();

                if ((character == 8 || character == 127) && count>0)
                {
                    count = count - 1;
                }

                else if (character == 13)
                {
                     data->buffer[count] = '\0';
                     return;
                }
                else if (character >= 32 && count < MAX_CHARS)
                {
                     data->buffer[count] = character;
                     count = count+1;
                }
                else if (count == MAX_CHARS)
                {
                      data->buffer[count++] = '\0';
                      return;
                }
            }

}

void parseFields(USER_DATA *data)
{
    uint8_t i;
    data->fieldCount = 0;
    //alpha, numeric and delimiter
    char prevChar = 'd';

    for(i=0; ((data->buffer[i] != '\0') && (data->fieldCount < MAX_FIELDS)); i++)
    {
        //If data->buffer[i] > A and Z || data->buffer[i] > a and z, the fieldType would be 'a'
        /*Ascii values
         * A=65, Z=90, a=97, z=122
         */
        if (((data->buffer[i] >= 65) && (data->buffer[i] <= 90) || (data->buffer[i] >= 97) && (data->buffer[i] <= 122)))
        {
            if (prevChar == 'd')
            {
                data->fieldPosition[data->fieldCount] = i;
                data->fieldType[data->fieldCount] = 'a';
                data->fieldCount = data->fieldCount + 1;
                prevChar = 'a';
            }
        }
        //If data->buffer[i] > 1 and 9
        /* Ascii values
         * '1' = 48, '9' = 57
         */
        else if ((data->buffer[i] >= 48) && (data->buffer[i] <= 57))
        {
            if (prevChar=='d')
            {
                data->fieldPosition[data->fieldCount] = i;
                data->fieldType[data->fieldCount] = 'n';
                data->fieldCount = data->fieldCount + 1;
                prevChar = 'n';
            }
        }
        else
        {
            data->buffer[i] = '\0';
            prevChar = 'd';
        }

    }
}

int compare_strings(const char *str1, const char *str2) {
    // Loop through each character of the strings
    while (*str1 != '\0' && *str2 != '\0') {
        // If characters are different, return a non-zero value
        if (*str1 != *str2) {
            return 0; // Strings are not equal
        }
        str1++;
        str2++;
    }

    // If both strings have reached the null character, they're equal
    // If one string is longer than the other, they are not equal
    return (*str1 == '\0' && *str2 == '\0');
}

int alphabetToInteger(char* stringToConvert)
{
    int result = 0;
    int j=0;
    int digit =0;
    for (j=0; j!='\0'; j++)
    {
        digit = stringToConvert[j]-'0'; //'0' = 48
        result = result * 10 + digit;
        return result;
    }
}

//Returns the value of a field requested if the field number is in range or NULL otherwise.
char* getFieldString(USER_DATA *data, uint8_t fieldNumber)
{
    //check if the requested field number
    if (fieldNumber <= data->fieldCount)
    {
        //first go to the string, then access the index of the string pointed to the field Number
        //return data->buffer[data->fieldPosition[fieldNumber]];
        //atoi is used to convert string to integer
        return &data->buffer[data->fieldPosition[fieldNumber]];
    }
}

//Returns the integer value of the field if the field number is in range and the field type is
//numeric or 0 otherwise.
int32_t getFieldInteger(USER_DATA *data, uint8_t fieldNumber)
{
    if ((fieldNumber < data->fieldCount) && (data->fieldType[fieldNumber] == 'n'))
    {
       return alphabetToInteger(&data->buffer[data->fieldPosition[fieldNumber]]);
    }
}

bool isCommand(USER_DATA *data, const char strCommand[], uint8_t minArguments)
{
    if ((compare_strings(strCommand, &data->buffer[data->fieldPosition[0]])) && (data->fieldCount-1 >= minArguments))
    {
        return true;
    }
    else
    {
        return false;
    }
}

