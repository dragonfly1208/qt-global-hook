#include "Hook.h"  
#include <atlstr.h>
#include <tchar.h>
//共享内存变量
#pragma data_seg("MyHooks")  
HWND g_lhDisplayWnd = NULL;

HHOOK g_hCallWnd = NULL;
HHOOK g_hCallWndRet = NULL;
HHOOK g_hCBT = NULL;
HHOOK g_hForegroundIdle = NULL;
HHOOK g_hGetMsg = NULL;
HHOOK g_hJournalPlayback = NULL;
HHOOK g_hJournalRecord = NULL;
HHOOK g_hMessage = NULL;
HHOOK g_hShell = NULL;
HHOOK g_hSysMsg = NULL;
HHOOK g_hKeyboard = NULL;
HHOOK g_hLowLevelKeyboard = NULL;

HHOOK g_hMouse = NULL;
HHOOK g_hLowLevelMouse = NULL;

#pragma data_seg()  
#pragma comment(linker,"/section:.MyHooks,rws")
int indexLog = 0;


//钩子函数的格式 LRESULT CALLBACK 函数名(int 钩子类型, WPARAM wParam, LPARAM lParam);
//处理鼠标的钩子函数  
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
//处理键盘的钩子函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

//这个是获取DLL的内存地址，可以重复使用，当做模版  
HMODULE WINAPI ModuleFromAddress(PVOID pv);


static BOOL mu = FALSE;
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*CString str;
	CString wp;
	switch (wParam)
	{
	case WM_LBUTTONDOWN:wp = _T("LD"); break;
	case WM_LBUTTONUP:wp = _T("LU"); break;
		//case WM_MOUSEMOVE:wp = _T("MM"); break;
		//case WM_NCMOUSEMOVE:wp = _T("MM"); break;
	case WM_MOUSEWHEEL:wp = _T("MW"); break;
	case WM_MOUSEHWHEEL:wp = _T("MHW"); break;
	case WM_RBUTTONDOWN:wp = _T("RD"); break;
	case WM_RBUTTONUP:wp = _T("RU"); break;
	default:
		return ::CallNextHookEx(g_hLowLevelMouse, nCode, wParam, lParam);
		break;
	}
	str.Format(_T("[%04X] MouseProc :nCode=%d,%d %s  .\n"), indexLog++, nCode, wParam, wp.GetBuffer());
	OutputDebugString(str);
	*/

	/*if (wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE) {
	//OutputDebugString(_T("MouseProc :MOUSEMOVE.\n"));
	return 1;
	}else */ /*if (wParam != WM_MOUSEMOVE && wParam !=  WM_NCMOUSEMOVE) {
	CString str;
	str.Format(_T("[%04X] MouseProc :nCode=%d,%X.\n"),indexLog++, nCode, (long)wParam);
	OutputDebugString(str);
	}else {
	if (mu)
	return 1;
	}
	//OutputDebugString(CString("MouseProc after SendMessage.\n"));
	if (wParam == WM_LBUTTONUP ) {//|| wParam == WM_LBUTTONDOWN
	OutputDebugString(CString("MouseProc WM_LBUTTONUP.\n"));
	mu = true;
	return 1;
	}*/
	if (wParam == WM_LBUTTONUP) {
		OutputDebugString(_T("MouseProc WM_LBUTTONUP"));
	}
	if (g_lhDisplayWnd) {
		
		::SendMessage(g_lhDisplayWnd, WM_USER + 100 + WH_MOUSE, wParam, lParam);
	}
		
	return ::CallNextHookEx(g_hMouse, nCode, wParam, lParam);
}
static bool isUpBtn = false;
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//CString str;
	//str.Format(_T("[%04X] LowLevelMouseProc :nCode=%d,%X.\n"), indexLog++, nCode, (long)wParam);
	//OutputDebugString(str);
	//if (isUpBtn) {
	//	return 1;
	//}
	if (!isUpBtn && wParam == WM_LBUTTONUP) {
		OutputDebugString(_T("LowLevelMouseProc WM_LBUTTONUP\n"));
		//isUpBtn = true;
		//return 0;
	}
	if (g_lhDisplayWnd)
		::SendMessage(g_lhDisplayWnd, WM_USER + 100 + WH_MOUSE_LL, wParam, lParam);

	return ::CallNextHookEx(g_hLowLevelMouse, nCode, wParam, lParam);
}

MOUSEKEYBOARDHOOK_API void SetHookMainHwnd(HWND hwnd) {
	g_lhDisplayWnd = hwnd;
}
#define SETHOOK(id,type,thread) if (!g_h##type && (hTypes&(1<< id))){\
OutputDebugString(_T("Sethook "#type" \n"));\
g_h##type = SetWindowsHookEx(id, type##Proc, ModuleFromAddress(type##Proc), thread);\
if(!g_h##type){ OutputDebugString(_T("Sethook "#type" err \n"));} else{retMask|=(1<< id);}}

#define SETHOOK2(id,type,thread,func) if (!g_h##type){\
OutputDebugString(_T("Sethook "#type" \n"));\
g_h##type = SetWindowsHookEx(id, func, ModuleFromAddress(func), thread);\
if(!g_h##type){ OutputDebugString(_T("Sethook "#type" err \n"));}}

#define UNSETHOOK(id,hook) if (hook&& (hTypes&(1<< id))){OutputDebugString(_T("Unhook "#hook"\n"));b = ::UnhookWindowsHookEx(hook);hook = NULL;\
if (!b) { OutputDebugString(_T("Unhook "#hook" err\n")); }else{retMask|=(1<< id);}}


//安装钩子函数  
MOUSEKEYBOARDHOOK_API unsigned int SetHook(HWND hWnd,unsigned int hTypes)
{
	HRESULT hr = 0;
	DWORD   dwThread = 0;
	unsigned int retMask = 0;

	//hWnd = GetShellWindow();
	if (hWnd) {
		dwThread = GetWindowThreadProcessId(hWnd, NULL);//g_ExpWnd
	}
	//SETHOOK(WH_MOUSE, Mouse, dwThread);

	SETHOOK(WH_MOUSE_LL, LowLevelMouse, dwThread)
	
	return retMask;
}

MOUSEKEYBOARDHOOK_API unsigned int UnSetHook(unsigned int hTypes)
{
	unsigned int retMask = 0;
	BOOL b = 1;
	//UNSETHOOK(g_hCallWnd);
	//UNSETHOOK(g_hCallWndRet);
	//UNSETHOOK(g_hCBT);
	//UNSETHOOK(g_hForegroundIdle);
	//UNSETHOOK(g_hGetMsg);
	//UNSETHOOK(g_hJournalPlayback);
	//UNSETHOOK(g_hJournalRecord);
	//UNSETHOOK(g_hMessage);
	//UNSETHOOK(g_hShell);

	UNSETHOOK(WH_MOUSE,g_hMouse);
	UNSETHOOK(WH_MOUSE_LL,g_hLowLevelMouse);

	return retMask;
}
HINSTANCE hInstanceHandle = NULL;   //DLL模块句柄
HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
	//MEMORY_BASIC_INFORMATION mbi;
	//if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
	//	return (HMODULE)mbi.AllocationBase;
	//} else {
	//	return NULL;
	//}
	if (!hInstanceHandle) {
		//hInstanceHandle = AfxGetInstanceHandle();
		hInstanceHandle = GetModuleHandle(_T("HOOK.dll"));
	}
	return hInstanceHandle;
}