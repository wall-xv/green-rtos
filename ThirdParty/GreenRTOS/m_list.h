
#ifndef LG_DLIST_H_ojkhsa049328yt924py45428o7kjye543w9u
#define LG_DLIST_H_ojkhsa049328yt924py45428o7kjye543w9u

typedef struct gg_dlisthead{
	struct gg_dlisthead* prev;
	struct gg_dlisthead* next;
}gg_dlisthead,* gg_pdlisthead;

#define DLIST_STRUCT(node, type, member) \
   ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

#define DLIST_IS_EMPTY(list)	((list)->next == 0)
#define DLIST_IS_EMPTY_C(list) 	((list)->next == (list))

#define DLIST_INIT(list)		\
	{(list)->prev=(list)->next=NULL;}
	
#define DLIST_INIT_C(list)		\
	{(list)->next = (list)->prev = (list);}

#define DLIST_ADD(index, list)		\
	if ((index) && (list))	{	\
		if ((index)->next) (index)->next->prev = (list);	\
		(list)->next = (index)->next;	\
		(index)->next = (list);	\
		(list)->prev = (index);	}
		
#define DLIST_ADD_C(index, list)		\
	if ((index) && (list))	{	\
		(index)->next->prev = (list);		\
		(list)->next = (index)->next;		\
		(index)->next = (list);			\
		(list)->prev = (index);	}

#define DLIST_ADD_PRE(index, list)		\
	if ((index) && (list))	{	\
		(list)->prev = (index)->prev;	\
		if ((index)->prev) (index)->prev->next = (list);	\
		(list)->next = (index);	\
		(index)->prev = (list);	}

#define DLIST_ADD_PRE_C(index, list)		\
	if ((index) && (list))	{	\
		(index)->prev->next = (list);	\
		(list)->prev = (index)->prev;	\
		(index)->prev = (list);			\
		(list)->next = (index);	}

#define DLIST_DEL(list)	{ \
	if ((list)->next) (list)->next->prev = (list)->prev;	\
	if ((list)->prev) (list)->prev->next = (list)->next;	\
	(list)->next = (list)->prev = NULL;}

#define DLIST_DEL_C(list)	{ \
	(list)->next->prev = (list)->prev;	\
	(list)->prev->next = (list)->next;	\
	(list)->next = (list)->prev = (list);	}

#define DLIST_ADD_ALT(index, list, nprev, pnext)		\
	if ((index) && (list))	{	\
		if ((index)->pnext) (index)->pnext->nprev = (list);	\
		(list)->pnext = (index)->pnext;	\
		(index)->pnext = (list);	\
		(list)->nprev = (index);	}

#define DLIST_DEL_ALT(list, nprev, pnext)	{ \
	if ((list)->pnext) (list)->pnext->nprev = (list)->nprev;	\
	if ((list)->nprev) (list)->nprev->next = (list)->pnext;	\
	(list)->pnext = NULL;	\
	(list)->nprev = NULL; }
	
#define SLIST_RADD(index, list){		\
	(list)->next=(index);		\
	(index)=(list);	}

#define SLIST_ADD(index, list){		\
	(list)->next=(index)->next;		\
	(index)->next=(list);	}

#define SLIST_RDEL(index, list){		\
	(index)=(list)->next;		\
	(list)->next=NULL;	}
	
#define SLIST_DEL(index, list){		\
	(index)->next=(list)->next;		\
	(list)->next=NULL;	}

#endif

