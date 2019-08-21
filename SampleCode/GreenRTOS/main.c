
/* Standard includes. */
#include <stdlib.h>

/* Nuvoton includes. */
#include "nuc970.h"
#include "sys.h"
#include "etimer.h"

/* Scheduler includes. */
#include "grtos.h"

static void prvSetupHardware( void );

/*-----------------------------------------------------------*/
m_pgrtos_sem 		g_sem = NULL;

#define LONG_TIME 0xffff
int timer_count = 0;
int timer0_count = 0;
int mutex_data[2] = {0, ~0};

void ETMR0_IRQHandler(void)
{
		M_GRTOSSemGave(g_sem);
		
    ETIMER_ClearIntFlag(0);
		
		timer0_count ++;
		if (timer0_count % 10000 == 0)
		{
			sysprintf("Timer0 count %d,tick %d\r\n", timer0_count, M_GRTOSGetTickCount());
		}
}

static void vTimerTask( void *pvParameters )
{
    /* Parameters are not used. */
    ( void ) pvParameters;

 		g_sem		= M_GRTOSSemCreate("Timer0", 0, 1); 

    outpw(REG_CLK_PCLKEN0, inpw(REG_CLK_PCLKEN0) | (1 << 4)); // Enable ETIMER0 engine clock
    // Set timer frequency to 1HZ
    ETIMER_Open(0, ETIMER_PERIODIC_MODE, 5000);

  	// Enable timer interrupt
    ETIMER_EnableInt(0);
    sysInstallISR(HIGH_LEVEL_SENSITIVE | IRQ_LEVEL_1, ETMR0_IRQn, (PVOID)ETMR0_IRQHandler);
    //sysSetLocalInterrupt(ENABLE_IRQ);
    sysEnableInterrupt(ETMR0_IRQn);

    // Start Timer 0
    ETIMER_Start(0);
	
    sysprintf("\nTimer Task is running ...\n\n");
	
    for( ;; ) {
				// Block waiting for the semaphore to become available.
        if( M_GRTOSSemTake( g_sem, LONG_TIME ) == EC_OK )
        {					
					 if (timer_count % 10000 == 0)
					 {
							sysprintf("Timer count %d,tick %d\r\n", timer_count, M_GRTOSGetTickCount());
					 }
           timer_count ++;
					 
					 M_GRTOSMutexLock();
					 if (mutex_data[0] != ~mutex_data[1])
					 {
							sysprintf("Mutex data %x %x\r\n", mutex_data[0], mutex_data[1]);
					 }
					 M_GRTOSMutexUnlock();
					 
#if 0					 
					 //EXCEPTION dump test
					 if (timer_count == 20000)
					 {
							M_GRTOS_TASK_MAIN ef;
						  ef = (M_GRTOS_TASK_MAIN)0x30000;
							ef(0);
					 }
#endif					 
				}		
    }
}


static void vSleepTask( void *pvParameters )
{
	int sleep_count = 0;
	/* Parameters are not used. */
	( void ) pvParameters;

	for( ;; ) {
		sysprintf("Sleep count %d,tick %d\r\n", sleep_count, M_GRTOSGetTickCount());
		M_GRTOSSleep(1000);
		sleep_count ++;
		M_GRTOSMutexLock();
		mutex_data[1] = sleep_count;
		mutex_data[0] = ~mutex_data[1];
	 M_GRTOSMutexUnlock();		
	}
}
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
    /* Setup the hardware for use with the Keil demo board. */
    prvSetupHardware();

    sysprintf("\nstart the FreeRTOS demo\n");

    M_GRTOSTaskCreate( vTimerTask, "Timer", 1024, NULL, 10, NULL );
    M_GRTOSTaskCreate( vSleepTask, "Sleep", 1024, NULL, 10, NULL );
	
    M_GRTOSStartScheduler();

    /* Should never reach here!  If you do then there was not enough heap
    available for the idle task to be created. */
    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Perform the hardware setup required.  This is minimal as most of the
    setup is managed by the settings in the project file. */

    sysDisableCache();
    sysFlushCache(I_D_CACHE);
    sysEnableCache(CACHE_WRITE_BACK);
    sysInitializeUART();
    sysSetLocalInterrupt(ENABLE_IRQ);

}

