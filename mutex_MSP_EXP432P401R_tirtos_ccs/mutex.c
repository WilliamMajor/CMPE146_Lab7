
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>

#define TASKSTACKSIZE   512

Void task1Fxn(UArg arg0, UArg arg1);
Void task2Fxn(UArg arg0, UArg arg1);

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

    while(true)
    {
        /* Get access to resource */
        Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);

        /* Do work */
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN5);
        Task_sleep(2000);
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN5);
        /* Unlock resource */

        Semaphore_post(semHandle);

    }
}

/*
 *  ======== task2Fxn ========
 */
Void task2Fxn(UArg arg0, UArg arg1)
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

   while(true)
   {
       Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);

       /* Do work */
       GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
       Task_sleep(2000);
       GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
       /* Unlock resource */

       Semaphore_post(semHandle);
   }
}
