#include <stdio.h>

#define DllExport __declspec(dllexport)
/*
cpusnid v1.0 (cpu id and serial number detector)
date: Jun. 12, 2009 5:12 PM
coder: oguz kartal

*/

void load_failed_str(char *s)
{
	const char *FAILED = "FAILED";
	int l=6;
	
	while(l--)
		*s++ = *FAILED++;

}


int is_support_cpuid()
{
	int r=0;
	__asm
	{
		pushfd
		pop eax
		mov ecx, eax
		xor eax, 0200000H
		push eax
		popfd
		pushfd
		pop ebx
		xor eax,ebx
		and eax,0200000H
		jnz restore_flag
		mov dword ptr[r], 1
restore_flag:
		push ecx
		popfd
	}

	return r;
}


int get_cpu_id(char *idstr)
{
	long int _edx=0,_eax=0;
	if (!is_support_cpuid())
		return 0;
	
	__asm
	{
		mov eax, 01H
		cpuid

		mov dword ptr[_edx], edx
		mov dword ptr[_eax], eax
	}

	sprintf(idstr,"%08X%08X",_edx,_eax);
	
	return 1;
}


int get_cpu_serial(char *serialstr)
{
	long int _eax=0,_edx=0,_ecx=0;
	if (!is_support_cpuid())
		return 0;

	__asm
	{
		mov eax, 01H
		cpuid

		mov dword ptr[_eax], eax

		mov eax, 03H
		cpuid

		mov dword ptr[_edx],edx
		mov dword ptr[_ecx],ecx

	}

	sprintf(serialstr,"%08X%08X%08X",_ecx,_edx,_eax);

	return 1;
}

DllExport void GetProcessorID(char *CpuId)
{
	if (!get_cpu_id(CpuId))
		load_failed_str(CpuId);
}

DllExport void GetProcessorSerialNumber(char *CpuSerial)
{
	if (!get_cpu_serial(CpuSerial))
		load_failed_str(CpuSerial);
}