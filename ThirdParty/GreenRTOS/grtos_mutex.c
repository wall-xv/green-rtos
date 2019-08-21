
/*
	mutex:
	
	disable/enable irq
*/

#include "grtos.h"

mt_s32 M_GRTOSMutexLock()
{
	M_GRTOSIRQEnable(0);

	RET_OK;
}

mt_s32 M_GRTOSMutexUnlock()
{
	M_GRTOSIRQEnable(1);

	RET_OK;
}
