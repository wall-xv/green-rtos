
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"

// call by irq
mt_s32 xTaskIncrementTick( void )
{
	g_grtos_global.tick_count ++;
	
#ifdef _DEBUG	
	M_GRTOSTaskIncCPUHold();
#endif
	
	//alway check switch
	RET_ERR;
}


m_tick_t M_GRTOSGetTickCount( void )
{
	return g_grtos_global.tick_count;
}

void M_GRTOSSleep( mt_u32 ms )
{
	m_pgrtos_task pc;

	//can not sleep in irq
	if (g_isr_mode)
	{
		return ;
	}
	
	if (pc)
	{
		M_GRTOSIRQEnable(0);
		
		pc =  M_GRTOSTaskCurrent();
		pc->wait_tick_out = M_GRTOSGetTickCount() + ms;
		
		M_GRTOSIRQEnable(1);

		//wait
		M_GRTOSTaskSwitch();	
	}
}
