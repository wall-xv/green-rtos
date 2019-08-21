
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"

void M_ExceptionTrace( mt_u32 addr)
{
	m_pgrtos_task pt;
	
	pt = M_GRTOSTaskCurrent();
	addr -= 8;

	M_GRTOSPrintf("addr=0x%8x"				, addr);
	M_GRTOSPrintf(";irq=%d"						,	g_isr_mode);
	M_GRTOSPrintf(";Task=%x"					,	pt);
	M_GRTOSPrintf(";Task name=%s"			,	pt->name);
	//M_GRTOSPrintf(";Task entry=%x"		,	pt->main);
	//M_GRTOSPrintf(";Task prio=%d"			,	pt->priority);
	M_GRTOSPrintf("\r\n");
	
	while (1)
	{
		;
	}
}

void Undefined_UserHandler( mt_u32 addr )
{
	M_GRTOSPrintf("EX:Undefined:");
	M_ExceptionTrace(addr);
}

void Prefetch_UserHandler( mt_u32 addr )
{
	M_GRTOSPrintf("EX:Prefetch:");
	M_ExceptionTrace(addr);
}

void Abort_UserHandler( mt_u32 addr )
{
	M_GRTOSPrintf("EX:Abort:");
	M_ExceptionTrace(addr);
}

void FIQ_UserHandler(mt_u32 addr)
{
	M_GRTOSPrintf("EX:FIQ:");
	M_ExceptionTrace(addr);
}
