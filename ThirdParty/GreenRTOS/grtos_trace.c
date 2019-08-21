
#define _CRT_SECURE_NO_WARNINGS 
#include <string.h>
#include "grtos.h"
//#include "sys.h"

void    sysPutChar (mt_u8 ucCh);
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
void M_GRTOSPrintf(const char* fmt, ...)
{
	int i;
	int size;
	va_list arg_list;
	char log[256];

	//M_GRTOSIRQEnable(0);
	
	va_start(arg_list, fmt);
	size = vsnprintf(log, 256, (const char *)fmt, arg_list);
	va_end(arg_list);
	
	log[size] = 0;
	
	for (i = 0; i < 256; i ++)
	{
		if (log[i])
		{
			sysPutChar(log[i]);
		}
		else
		{
			break;
		}
	}
	
	//M_GRTOSIRQEnable(1);
}

