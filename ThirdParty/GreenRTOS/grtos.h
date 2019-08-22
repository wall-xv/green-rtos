
/*
1 easy port: not modify bsp driver
2 only support base function:mm task sem mutex
3 can wake task by sem in irq 
*/

#ifndef	GRTOS_H_jdjoww
#define	GRTOS_H_jdjoww

#include "m_type.h"
#include "m_list.h"

typedef mt_u32 TaskFunction_t;
typedef mt_u32 StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#define configUSE_PREEMPTION (1)
#define configTICK_RATE_HZ (1000)
#define MD_MM_BUF_SIZE (1024*1024*2)

#define MD_GRTOS_NAME_SIZE	(16)
#define MD_GRTOS_FOREVER	(0xffffffff)

extern int			g_isr_mode ;

//u64 not support
typedef mt_u32 m_tick_t;
typedef void (*M_GRTOS_TASK_MAIN)(mt_pv pd);

typedef struct m_grtos_task  *m_pgrtos_task;

typedef struct m_grtos_sem
{
	mt_ch				name[MD_GRTOS_NAME_SIZE];
	mt_s32				value_cur;
	mt_s32				value_max;
} m_grtos_sem, *m_pgrtos_sem;

typedef struct m_grtos_task
{
	volatile mt_pv			stack_ptop;
	mt_pu8							stack_pbuf;
	mt_u32							stack_size;
	mt_u32							priority;
	mt_ch								name[MD_GRTOS_NAME_SIZE];
	M_GRTOS_TASK_MAIN		main;
	mt_pv								pdata;

	struct m_grtos_task* prev;
	struct m_grtos_task* next;

	//当前等待
	m_tick_t						wait_tick_out;
	m_pgrtos_sem				wait_psem;
	
	//执行计数
	mt_u16 							tick_cpu_hold_cur;
	mt_u16 							tick_cpu_hold_last;
	
	//
	mt_s8							err_code;
	mt_s8							need_delete;
	mt_s8							tcb_is_alloc;
	mt_s8							stack_is_alloc;
} m_grtos_task, *m_pgrtos_task;


typedef struct m_grtos_global
{
	//系统计时
	m_tick_t			tick_count;

	//资源的链表
	/*
		任务队列头，第一个任务是当前执行的
		定时器也在队列里面检查
	*/
	m_grtos_task		dlh_task;
	
	m_pgrtos_task		ph_idle;
} m_grtos_global, *m_pgrtos_global;

extern m_grtos_global	g_grtos_global;
extern m_pgrtos_task	g_task_pcur;

#ifdef __cplusplus
extern "C"
{
#endif 

mt_pv M_GRTOSMMAlloc(mt_u32 size);
void M_GRTOSMMFree(mt_pv pmm);

mt_s32 M_GRTOSTaskCreate(	M_GRTOS_TASK_MAIN main,
							mt_pcch name,
							const mt_u32 stack_size,
							const mt_pv  pdata,
							mt_u32 priority,
							mt_pv * phand);
m_pgrtos_task M_GRTOSTaskCurrent(void);
mt_s32 M_GRTOSTaskChangeCurrent( void );
mt_s32 M_GRTOSTaskSwitch(void);
void M_GRTOSStartScheduler(void);
mt_s32 M_GRTOSTaskIncCPUHold(void);
mt_s32 M_GRTOSTaskDumpCPUHold(void);
							
m_tick_t M_GRTOSGetTickCount( void );
void M_GRTOSSleep( mt_u32 ms );

							
mt_s32 M_GRTOSSemInit(m_pgrtos_sem pm, mt_pcch name, mt_s32 val, mt_s32 max);
mt_s32 M_GRTOSSemDeinit(m_pgrtos_sem pm);
m_pgrtos_sem M_GRTOSSemCreate(mt_pcch name, mt_s32 val, mt_s32 max);
mt_s32 M_GRTOSSemDestory(m_pgrtos_sem pm);
mt_s32 M_GRTOSSemTake(m_pgrtos_sem pm, mt_u32 tout);
mt_s32 M_GRTOSSemGave(m_pgrtos_sem pm);
mt_s32 M_GRTOSSemValue(m_pgrtos_sem pm);

mt_s32 M_GRTOSMutexLock(void);
mt_s32 M_GRTOSMutexUnlock(void);
							
void M_GRTOSPrintf(const char* fmt, ...);
void M_GRTOSIRQEnable( int en );

#ifdef __cplusplus
}
#endif 
							
							
#endif

