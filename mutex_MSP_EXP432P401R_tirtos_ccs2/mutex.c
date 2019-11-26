
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>

#define TASKSTACKSIZE   1024

Void task1Fxn(UArg arg0, UArg arg1);
Void task2Fxn(UArg arg0, UArg arg1);

UART_Handle uart;
UART_Params uartParams;


Int resource = 0;
Int finishCount = 0;
UInt32 sleepTickCount;

Task_Struct task1Struct, task2Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];
Semaphore_Struct semStruct;
Semaphore_Handle semHandle;

/*
 *  ======== main ========
 */
int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;
    Semaphore_Params semParams;

    /* Call driver init functions */
    Board_init();

    UART_init();

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

    /* Construct writer/reader Task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    taskParams.priority = 1;
    Task_construct(&task1Struct, (Task_FuncPtr)task1Fxn, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    taskParams.priority = 2;
    Task_construct(&task2Struct, (Task_FuncPtr)task2Fxn, &taskParams, NULL);

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&semStruct, 1, &semParams);

    /* Obtain instance handle */
    semHandle = Semaphore_handle(&semStruct);

    /* We want to sleep for 10000 microseconds */
    sleepTickCount = 10000 / Clock_tickPeriod;

    BIOS_start();    /* Does not return */
    return(0);
}

/*
 *  ======== task1Fxn ========
 */
Void task1Fxn(UArg arg0, UArg arg1)
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5); //Port 1 Pin 0 is only supplying 0.3v. I suspect the pin is bad I have placed a jumper from P1_5 to the LED to continue with the lab.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);

    char        input;

    if (uart == NULL)
        while (1);

    while (1)
    {
        UART_read(uart, &input, 1);
        if(input == '1')
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5);
        else if(input == '0')
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);
    }
}

/*
 *  ======== task2Fxn ========
 */
Void task2Fxn(UArg arg0, UArg arg1)
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    const char  buttonPressed[] = "CLOSE\n";
    const char  buttonNotPressed[] = "OPEN\n";
    int         count = 0;
    uint8_t     previousValue;


    if (uart == NULL)
        while (1);


    while(1)
    {
        if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_LOW)
        {
            if(previousValue != GPIO_INPUT_PIN_LOW || count >= 20)
            {
                UART_write(uart, buttonPressed, sizeof(buttonPressed));
                count = 0;
                previousValue = GPIO_INPUT_PIN_LOW;
            }
            else
                count++;
        }
        else if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == GPIO_INPUT_PIN_HIGH)
        {
            if(previousValue != GPIO_INPUT_PIN_HIGH || count >= 20)
            {
                UART_write(uart, buttonNotPressed, sizeof(buttonNotPressed));
                count = 0;
                previousValue = GPIO_INPUT_PIN_HIGH;
            }
            else
                count++;
}
        Task_sleep(100);
    }


}


