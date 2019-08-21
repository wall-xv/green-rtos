
#ifndef M_TYPEDEF_H_shduiewuqjhrui83289eir7928iqwejy8r8e93s
#define M_TYPEDEF_H_shduiewuqjhrui83289eir7928iqwejy8r8e93s

#ifdef __cplusplus
extern "C"
{
#endif 

#define __stdcall 	
#define M_PI       3.14159265358979323846f	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	typedef
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned char		 		mt_u8;
typedef unsigned char*		 		mt_pu8;
typedef signed char         		mt_s8;
typedef signed char*         		mt_ps8;
typedef unsigned short      		mt_u16;
typedef unsigned short*      		mt_pu16;
typedef signed short        		mt_s16;
typedef signed short*        		mt_ps16;
typedef unsigned int       			mt_u32;
typedef unsigned int*       		mt_pu32;
typedef signed int         			mt_s32;
typedef signed int*         		mt_ps32;
#ifdef WIN32
typedef unsigned __int64    		mt_u64;
typedef __int64             		mt_s64;
#else
typedef unsigned long long  		mt_u64;
typedef long long           		mt_s64;
#endif
typedef float               		mt_float;
//typedef double              		mt_double;
typedef float              			mt_double;

typedef void                		mt_void;
typedef void*                		mt_pv;
typedef const void*            	mt_pcv;
typedef char                		mt_ch;
typedef char*               		mt_pch;	
typedef const char*         		mt_pcch;	
typedef mt_u16              		mt_uc;
typedef mt_u16*             		mt_puc;
typedef const mt_u16*       		mt_pcuc;
typedef long       			 		mt_bool;

#define EC_OK			(0)
#define EC_ERR			(-1)
#define EC_ECMD			(-2)
#define EC_EDAT			(-3)
#define EC_EBUS			(-4)
#define EC_RETRY		(-5)
#define EC_TOUT			(-6)

#define MD_FALSE		(0)
#define MD_TRUE			(1)
#define MD_NULL			(0)

#define MD_PAI2A(p)		(p*180/M_PI)
#define MD_A2PAI(p)		(p*M_PI/180)

#define TFL(s)			

#define RET(x)			{return (x);}
#define RET_OK			{return EC_OK;}
#define RET_ERR			{TFL("RET_ERR");return EC_ERR;}
#define RET_ECMD		{TFL("RET_ECMD");return EC_ECMD;}
#define RET_EDAT		{TFL("RET_EDAT");return EC_EDAT;}
#define RET_EBUS		{TFL("RET_EBUS");return EC_EDAT;}
#define RET_NUL			{TFL("RET_NUL");return MD_NULL;}

#define CHK_NUL(x)		{if(!(x)) return MD_NULL;}
#define CHK_ERR(x)		{if(!(x)) {TFL("CHK_ERR");return EC_ERR;}}
#define AST_NUL(x)		{if(!(x)) return MD_NULL;}
#define AST_ERR(x)		{if(!(x)) {TFL("AST_ERR");return EC_ERR;}}

#define AST(x)			{if(!(x)){*(char*)0 = 0;}}

#define MD_MIN(x,y) ((x)<(y)?(x):(y))
#define MD_MAX(x,y) ((x)>(y)?(x):(y))
#define MD_ABS(x,y) ( (x)>(y) ? ((x)-(y)) : ((y)-(x)) )
#define MD_RGB(r,g,b)          ((mt_u32)(((mt_u8)(r)|((mt_u16)((mt_u8)(g))<<8))|(((mt_u16)(mt_u8)(b))<<16)))

#define STRUCT_ENTRY(node, type, member) \
   ((type)((char *)(node) - (unsigned long)(&((type)0)->member)))


#define STRUCT_POFFSET(type, member) \
   ((mt_pv)(&((type)0)->member))

#include "m_list.h"


#ifdef __cplusplus
}
#endif 

#endif
