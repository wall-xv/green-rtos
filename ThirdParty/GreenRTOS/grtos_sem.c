
/*

*/
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"

#define L_TRACE_T(...)
#define L_TRACE_G(...)

mt_s32 M_GRTOSSemInit(m_pgrtos_sem pm, mt_pcch name, mt_s32 val, mt_s32 max)
{
	CHK_ERR(pm);

	pm->value_cur = val;
	pm->value_max = max;
	pm->name[0] = 0;

	strncpy(pm->name, name, MD_GRTOS_NAME_SIZE);
	RET_OK;
}

m_pgrtos_sem M_GRTOSSemCreate(mt_pcch name, mt_s32 val, mt_s32 max)
{
	m_pgrtos_sem pm;
	
	pm = M_GRTOSMMAlloc(sizeof(*pm));
	if (pm)
	{	
		M_GRTOSSemInit(pm, name, val, max);
	}

	return pm;
}

mt_s32 M_GRTOSSemDeinit(m_pgrtos_sem pm)
{
	CHK_ERR(pm);
	
	//do nothine

	RET_OK;
}

mt_s32 M_GRTOSSemDestory(m_pgrtos_sem pm)
{
	CHK_ERR(pm)
	
	M_GRTOSMMFree(pm);

	RET_OK;
}

mt_s32 M_GRTOSSemValue(m_pgrtos_sem pm)
{
	mt_s32 val;
	CHK_ERR(pm)
	
	M_GRTOSIRQEnable(0);
	val = pm->value_cur;
	M_GRTOSIRQEnable(1);

	return val;
}

mt_s32 M_GRTOSSemTake(m_pgrtos_sem pm, mt_u32 tout)
{
	m_pgrtos_task pc;

	CHK_ERR(pm);

	M_GRTOSIRQEnable(0);
	//valid now
	if (pm->value_cur > 0)
	{
		L_TRACE_T("SEM %s got %d\r\n", pc->name, pm->value_cur);

		pm->value_cur --;
		
		M_GRTOSIRQEnable(1);

		RET_OK;
	}
	else	//not valid
	{
		pc =  M_GRTOSTaskCurrent();
		
		//record to task
		pc->wait_psem = pm;
		pc->wait_tick_out = M_GRTOSGetTickCount() + tout;

		L_TRACE_T("SEM %s wait %s=%d\r\n", pc->name, pm->name, pm->value_cur);

		M_GRTOSIRQEnable(1);
		
		//this task wait, switch to other task
		M_GRTOSTaskSwitch();

		//now I'm back
		L_TRACE_T("SEM %s wait done %s=%d\r\n", pc->name, pm->name, pm->value_cur);
		
		M_GRTOSIRQEnable(0);
		
		//clean task record
		pc->wait_psem = 0;
		//if timeouted
		if (pc->err_code == EC_TOUT)
		{
			//clean timeout
			pc->wait_tick_out = 0;
			
			M_GRTOSIRQEnable(1);
			
			//return 
			return EC_TOUT;
		}
		else	// value valid
		{
			pm->value_cur --;
			pc->wait_tick_out = 0;
			
			M_GRTOSIRQEnable(1);
			
			RET_OK;			
		}
	}

}

mt_s32 M_GRTOSSemGave(m_pgrtos_sem pm)
{	
	CHK_ERR(pm);

	M_GRTOSIRQEnable(0);

	pm->value_cur ++;
	if (pm->value_cur > pm->value_max)
	{
		pm->value_cur = pm->value_max;
	}
	
	M_GRTOSIRQEnable(1);

	L_TRACE_G("SEM %s gave %d\r\n", pm->name, pm->value_cur);
	
	//try switch
	M_GRTOSTaskSwitch();
	
	L_TRACE_G("SEM %s gave1 %d\r\n", pm->name, pm->value_cur);

	RET_OK;
}
