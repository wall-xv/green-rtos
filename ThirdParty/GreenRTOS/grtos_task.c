
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"
#include "portmacro.h"

#define L_TRACE_ISR(...)
#define L_TRACE_SW(...)
#define L_TRACE_SW_SEM(...)

m_grtos_global	g_grtos_global = {0};
m_pgrtos_global	g_pgrtos_global = &g_grtos_global;
m_pgrtos_task	pxCurrentTCB = 0;
int			g_isr_mode = 0;

#define M_GRTOSPrintf_d1(...)

mt_pu8 pxPortInitialiseStack( mt_pu8 pxTopOfStack, M_GRTOS_TASK_MAIN pxCode, mt_pv pvParameters );


static void M_GRTOSMain( void *pv )
{
	m_pgrtos_task pc;
	
	if (pv == 0)
	{
		return ;
	}
	
	pc = (m_pgrtos_task)pv;
	
	pc->main(pc->pdata);
	
	//delete myself
	pc->need_delete = 1;
	
	//delete when switch
	M_GRTOSTaskSwitch();
	
	while (1) {;}
}

mt_s32 M_GRTOSTaskInit(	m_pgrtos_task pc,
							M_GRTOS_TASK_MAIN main,
							mt_pcch name,
							const mt_u32 stack_size,
							const mt_pv  pdata,
							mt_u32 priority)
{
	CHK_ERR(pc);

	pc->main		= main;
	pc->name[0]		= 0;
	strncpy(pc->name, name, MD_GRTOS_NAME_SIZE);
	pc->stack_size	= stack_size;
	pc->pdata		= pdata;
	pc->priority	= priority;
	pc->tick_cpu_hold_cur = 0;
	pc->tick_cpu_hold_last = 0;
	
	//flags
	pc->err_code = 0;
	pc->need_delete = 0;
	pc->stack_is_alloc = 1;
	pc->tcb_is_alloc = 0;
	
	M_GRTOSIRQEnable(0);

	if (pxCurrentTCB == 0)
	{
		g_grtos_global.tick_count = 0;
		DLIST_INIT_C(&g_grtos_global.dlh_task);

		pxCurrentTCB = pc;
	}

	DLIST_INIT_C(pc);
	DLIST_ADD_C(&g_pgrtos_global->dlh_task, pc);

	//alloc stack
	pc->stack_pbuf = M_GRTOSMMAlloc(pc->stack_size);
	if (pc->stack_pbuf)
	{
		//init stack
		//pc->stack_ptop = pxPortInitialiseStack( pc->stack_pbuf + pc->stack_size, pc->main, pc->pdata );
		pc->stack_ptop = pxPortInitialiseStack( pc->stack_pbuf + pc->stack_size, M_GRTOSMain, pc );
	}

	M_GRTOSIRQEnable(1);

	RET_OK;
}

mt_s32 M_GRTOSTaskCreate(	M_GRTOS_TASK_MAIN main,
							mt_pcch name,
							const mt_u32 stack_size,
							const mt_pv  pdata,
							mt_u32 priority,
							mt_pv * phand)
{
	m_pgrtos_task pc;

	pc = M_GRTOSMMAlloc(sizeof(*pc));
	if (pc)
	{
		M_GRTOSTaskInit(pc, main, name, stack_size, pdata, priority);
		pc->tcb_is_alloc = 1;
	}

	if (phand)
	{
		*phand = pc;
	}

	RET_OK;
}

static mt_s32 M_GRTOSTaskDelete( m_pgrtos_task pc )
{
	CHK_ERR(pc);

	DLIST_DEL_C(pc);
	
	if (pc->stack_is_alloc)
	{
		M_GRTOSMMFree(pc->stack_pbuf);
	}
	
	if (pc->tcb_is_alloc)
	{
		M_GRTOSMMFree(pc);
	}	

	RET_OK;
}

m_pgrtos_task M_GRTOSTaskCurrent()
{
	return pxCurrentTCB;
}

/*
	always call in irq，need not disable irq
	can not be call out side irq
*/
mt_s32 vTaskSwitchContext()
{
	m_pgrtos_task pt;
	m_pgrtos_task pb;
	m_pgrtos_task pr;
	mt_s32 ret;
	
	pr = 0;
	ret = EC_OK;

	M_GRTOSPrintf_d1("M_GRTOSTaskSwitch 01\r\n");
	/*
		找出最需要执行的task
	*/
	pt = (m_pgrtos_task)g_grtos_global.dlh_task.next;
	while ( 1 )
	{
		pb = pt;
		pt = (m_pgrtos_task)pt->next;
		
		if (pb == (m_pgrtos_task)&g_grtos_global.dlh_task)
		{
			break;
		}
		
		//delete 
		if (pb->need_delete)
		{
			if (pxCurrentTCB == pb)
			{
				pxCurrentTCB = g_grtos_global.ph_idle;
			}
			
			M_GRTOSTaskDelete(pb);
			continue;
		}
		
		//all set to OK
		pb->err_code = EC_OK;
		
		// wait for sem
		if (pb->wait_psem)
		{
			//not valid and not timeout
			if (pb->wait_psem->value_cur == 0
					&& pb->wait_tick_out > g_grtos_global.tick_count
			)
			{
				//can not run
				continue;
			}	
			else	// valid or timeout
			{
				// can run, set err
				pb->err_code = EC_TOUT;
				
				L_TRACE_SW_SEM("Switch %s SEM %s val %d isr %d\r\n", 
					pb->name, 
					pb->wait_psem->name,
					pb->wait_psem->value_cur,
					g_isr_mode);
			}
		}
		// wait for sleep timeout
		else if (pb->wait_tick_out > 0)
		{
			// not timeout
			if (pb->wait_tick_out > g_grtos_global.tick_count)
			{
				continue;
			}
			else
			{
				//can run, set err
				pb->err_code = EC_TOUT;
			}
		}
		else
		{
			//can run
		}

		// frist can be run
		if (pr == 0)
		{
			pr = pb;
		}
		else
		{
			// priority higher
			if (pb->priority > pr->priority)
			{
				pr = pb;
			}
		}	
	}

	M_GRTOSPrintf_d1("M_GRTOSTaskSwitch 30 %x\r\n", pr);
	
	//no one need to run or Current need to run
	if (pr == 0)
	{
		//not occur, idle is always need run
	}
	else if (pr == pxCurrentTCB)
	{
		//also me, maybe e.g. tick timer check
	}	
	else
	{
		// other need to run
		pxCurrentTCB = pr;
		L_TRACE_SW("Switch to %s\r\n", pr->name);
	}

	return ret;
}

mt_s32 M_GRTOSTaskSwitch( void )
{
	L_TRACE_ISR("isr_mode %d\r\n", g_isr_mode);
	if (g_isr_mode == 0)
	{
		//outside irq, call soft int
		vPortYield();
	}
	else
	{
		//inside irq, do switch
		vTaskSwitchContext();
	}
	
	RET_OK;
}

void M_GRTOSIRQEnable( int en )  
{
	/*
		only can be disable ouside IRQ
	*/
	if (g_isr_mode == 0)
	{
		if ( en )
		{
			vPortExitCritical();
		}
		else
		{
			vPortEnterCritical();
		}	
	}
}

static void MainIdel_SYS( void *pvParameters )
{
	while( 1 ) {
  }
}

#ifdef _DEBUG	
static void MainDebug_SYS( void *pvParameters )
{
	while( 1 ) {
		M_GRTOSTaskDumpCPUHold();

		M_GRTOSSleep( 10000 );
	}
}
#endif

mt_s32 xPortStartScheduler( void );
void M_GRTOSStartScheduler()
{
  M_GRTOSTaskCreate( MainIdel_SYS, 	"SYS_Idel", 	256, 	NULL, 0, (void**)&g_grtos_global.ph_idle );

#ifdef _DEBUG	
  M_GRTOSTaskCreate( MainDebug_SYS, "SYS_Debug", 	1024, NULL, 1, NULL );
#endif
	
	xPortStartScheduler();
}

/*
	
*/
mt_s32 M_GRTOSTaskIncCPUHold()
{
	m_pgrtos_task pt;
	
	if (M_GRTOSGetTickCount() % 1000 == 0)
	{
		//M_GRTOSIRQEnable(0);
		
		//清理所有计数
		pt = (m_pgrtos_task)g_grtos_global.dlh_task.next;
		while ( pt != &g_grtos_global.dlh_task )
		{
			pt->tick_cpu_hold_last = pt->tick_cpu_hold_cur;
			pt->tick_cpu_hold_cur = 0;
			
			pt = (m_pgrtos_task)pt->next;
		}
		
		//M_GRTOSIRQEnable(1);
	}
	
	if (pxCurrentTCB)
	{
		pxCurrentTCB->tick_cpu_hold_cur ++;
	}
	
	RET_OK;
}

//打印CPU占用率,可以显示其他的状态用于调试,
mt_s32 M_GRTOSTaskDumpCPUHold()
{
	m_pgrtos_task pt;
	
	M_GRTOSIRQEnable(0);

	M_GRTOSPrintf("<--- Task cpu usage tick %d\r\n", M_GRTOSGetTickCount());
	
	//清理所有计数
	pt = (m_pgrtos_task)g_grtos_global.dlh_task.next;
	while ( pt != &g_grtos_global.dlh_task )
	{
		M_GRTOSPrintf("%16s:%d\r\n", pt->name, pt->tick_cpu_hold_last);
		
		pt = (m_pgrtos_task)pt->next;
	}
	
	M_GRTOSPrintf("--->\r\n");

	M_GRTOSIRQEnable(1);

	RET_OK;
}



