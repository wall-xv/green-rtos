
/*
	
*/

#include "grtos.h"

typedef struct m_grtos_mmb
{
	struct m_grtos_mmb* prev;
	struct m_grtos_mmb* next;
	mt_u32 size;
} m_grtos_mmb, *m_pgrtos_mmb;

static mt_u8 g_mm_buf[MD_MM_BUF_SIZE];
static mt_u32 g_mm_empty = MD_MM_BUF_SIZE;
static mt_u32 g_mm_alloced = 0;
static m_grtos_mmb g_grtos_mmb_free = {0};

static void M_GRTOSMMInit()
{
	if (g_grtos_mmb_free.next)
	{
		return;
	}

	DLIST_INIT_C(&g_grtos_mmb_free);
}

mt_pv M_GRTOSMMAlloc(mt_u32 size)
{
	mt_pu8 pm;
	m_pgrtos_mmb pt,pb;

	M_GRTOSIRQEnable(0);

	M_GRTOSMMInit();
	
	//freeed
	pt = g_grtos_mmb_free.next;
	while (pt != &g_grtos_mmb_free)
	{
		pb = pt;
		pt = pt->next;

		if (pb->size < size)
		{
			continue;
		}

		g_mm_empty -= pb->size;
		g_mm_empty -= sizeof(*pb);

		DLIST_DEL_C(pb);

		M_GRTOSIRQEnable(1);
		
		pm = (mt_pu8)pb;
		pm += sizeof(*pb);

		return pm;
	}

	//new
	pt = (m_pgrtos_mmb)(g_mm_buf + g_mm_alloced);
	DLIST_INIT_C(pt);
	pt->size = size;

	g_mm_alloced += size;
	g_mm_alloced += sizeof(*pt);

	M_GRTOSIRQEnable(1);

	pm = (mt_pu8)pt;
	pm += sizeof(*pb);
	
	//M_GRTOSPrintf("M_GRTOSMMAlloc %d %p\r\n", size, pm);

	return pm;
}

void M_GRTOSMMFree(mt_pv pmm)
{
	m_pgrtos_mmb pt,pb;
	mt_pu8 pm;
	
	if (pmm == 0)
	{
		return ;
	}

	pm = (mt_pu8)pmm;
	pm -= sizeof(*pt);

	pt = (m_pgrtos_mmb)pm;
	DLIST_INIT_C(pt);
	
	//sort by size
	pb = g_grtos_mmb_free.prev;
	while (pb != &g_grtos_mmb_free)
	{
		if (pb->size < pt->size)
		{
			break;
		}

		pb = pb->prev;
	} 

	//M_GRTOSPrintf("M_GRTOSMMFree %d %p\r\n", pt->size, pmm);
	DLIST_ADD_C(pb, pt);
}

