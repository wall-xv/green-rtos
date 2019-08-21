
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"
//#include "sys.h"

void    sysPutChar (mt_u8 ucCh);
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
char g_log[256];
void M_GRTOSPrintf(const char* fmt, ...)
{
	int i;
	int size;
	va_list arg_list;

	//M_GRTOSIRQEnable(0);
	
	va_start(arg_list, fmt);
	size = vsnprintf(g_log, 256, (const char *)fmt, arg_list);
	va_end(arg_list);
	
	g_log[size] = 0;
	
	for (i = 0; i < 256; i ++)
	{
		if (g_log[i])
		{
			sysPutChar(g_log[i]);
		}
		else
		{
			break;
		}
	}
	
	//M_GRTOSIRQEnable(1);
}

