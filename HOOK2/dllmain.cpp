// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
//#include <Windows.h>
//#include <cstdio>
//#include <iostream>
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif

#include <shellapi.h>
#include <ole2.h>
//#include <atlstr.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{


	switch (ul_reason_for_call)
	{
		
	case DLL_PROCESS_ATTACH:
	
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}


