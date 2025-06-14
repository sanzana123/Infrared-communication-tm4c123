#include <stdint.h>

#include "clock.h"
#include "uart0.h"
#include "wait.h"
#include "tm4c123gh6pm.h"
#include "debug.h"


#define MAX_CHARS 80
#define MAX_FIELDS 5

#define RED_LED_MASK     0x02 // PC4
#define BLUE_LED_MASK    0x04 // PE2
#define GREEN_LED_MASK   0x08 // PE3


/*typedef struct _USER_DATA
{
  char buffer[MAX_CHARS+1]; //stores the user input. Its size is 80. The +1 is for the null terminator
  uint8_t fieldCount;       //Keeps track of how many fields are found in the word that the user typed in
  uint8_t fieldPosition[MAX_FIELDS]; //Stores the starting index of each field in the buffer
  char fieldType[MAX_FIELDS]; //Stores the type of each field. Like 'a' for alpha, 'n' for numeric
} USER_DATA;*/

//struct _USER_DATA USER_DATA data;
struct _USER_DATA data;

volatile uint32_t systick_count = 0;


void initSystickTimer(void)
{
    // Set RELOAD
    //NVIC_ST_RELOAD_R |= 0x00ffffff;
    //40 Mhz = 1/(40 x 1000000) = 25ns
    NVIC_ST_RELOAD_R = 40000-1; // 40000-1 = 39999
    // Clear Current
    NVIC_ST_CURRENT_R = 0x0;
    // Set Control 4 MHZ Clock
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_CLK_SRC;
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN| NVIC_ST_CTRL_CLK_SRC; // enable counter and interrupt
}

uint32_t currentTime()
{
    return systick_count;
}

uint32_t deltaTime(uint32_t t0,uint32_t t1)
{
    return t1-t0;
}

void initHw(void)
{
    // Initialize system clock to 40 MHz
    //initSystemClockTo40Mhz();

    // Enable GPIO clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; //Clock for Port F
    _delay_cycles(3); // Wait for the GPIO clocks to stabilize

    // Configure LED and pushbutton pins
    GPIO_PORTF_DIR_R |= RED_LED_MASK; // PC4, PC5 as outputs
    GPIO_PORTF_DIR_R |= BLUE_LED_MASK | GREEN_LED_MASK; // PE2, PE3 as outputs


    GPIO_PORTF_DEN_R |= BLUE_LED_MASK | RED_LED_MASK | GREEN_LED_MASK; // Enable digital on PF2, PF1, PF3, PF4
}


void turnRedLedOn()
{
    //turn red led on
    GPIO_PORTF_DATA_R |= RED_LED_MASK;
}

void turnRedLedOff()
{
    //turn red led off
    GPIO_PORTF_DATA_R &= ~RED_LED_MASK;

}

void turnBlueLedOn()
{
    //turn blue led on
    GPIO_PORTF_DATA_R |= BLUE_LED_MASK;
}

void turnBlueLedOff()
{
    //turn blue led off
    GPIO_PORTF_DATA_R &= ~BLUE_LED_MASK;
}
void turnGreenLedOn()
{
    //turn green led on
    GPIO_PORTF_DATA_R |= GREEN_LED_MASK;
}

void turnGreenLedOff()
{
    //turn green led off
    GPIO_PORTF_DATA_R &= ~GREEN_LED_MASK;
}

void SystickHandler(void)
{
    systick_count = systick_count + 1;

    if (systick_count % 1000 == 0)
    {
        // GREEN = 00001000, PORxxTF= xxxxxxx
        if(GREEN_LED_MASK & GPIO_PORTF_DATA_R)
        {
            turnGreenLedOff();
        }
        else
        {
            turnGreenLedOn();
        }
    }

}


int main()
{
    initSystemClockTo40Mhz(); //ALWAYS SET THE SYSTEM CLOCK
    initUart0();
    initHw();
    initSystickTimer();

    while(1)
    {


        int result = 0;
            //GPIO_PORTF_DATA_R |= RED_LED_MASK;


            // Parse fields
        getsUart0(&data);
        parseFields(&data);
            // Echo back the parsed field data (type and fields)
            //#ifdef DEBUG
        uint8_t i;
        for (i = 0; i < data.fieldCount; i++)
        {
              putcUart0(data.fieldType[i]);
              putcUart0('\t');
              putsUart0(&data.buffer[data.fieldPosition[i]]);
              putcUart0('\n');
        }
            //#endif

            // Command evaluation
        bool valid = false;
            // set add, data → add and data are integers
        if (isCommand(&data, "set", 2))
        {
              int32_t add = getFieldInteger(&data, 1);
              int32_t data69870 = getFieldInteger(&data, 2);
              valid = true;
              // do something with this information
              result = add - data69870;
              if (result > 0)
              {
                  putsUart0("pass");
              }
              else
              {
                  putsUart0("fail");
              }

         }
            // alert ON|OFF → alert ON or alert OFF are the expected commands
         if (isCommand(&data, "alert", 1))
         {
              char* str = getFieldString(&data, 1);
              valid = true;
              // process the string with your custom strcmp instruction, then do something
         }
            //To test or demo
            // We enter: red on/off
         char* led_option = getFieldString(&data, 1);
         if (isCommand(&data, "red", 1))
         {
              valid = true;
              if (compare_strings(led_option, "on"))
              {
                  turnRedLedOn();
              }
              else if (compare_strings(led_option, "off"))
              {
                  turnRedLedOff();
              }
          }

        //      if(isCommand(&data, "led", 3))
        //      {
        //         valid = true;
        //         if (compare_strings(str_color, "blue"))
        //         {
        //             if (compare_strings(str_option, "on"))
        //             {
        //                 turnBlueLedOn();
        //             }
        //             else if(compare_strings(str_option, "off"))
        //             {
        //                 turnBlueLedOff();
        //             }
        //         }
        //      }

        //    }
              // Process other commands here
              // Look for error
        if (!valid)
        {
              putsUart0("Invalid Command\n");
        }
    }
}
