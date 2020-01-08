#include "MouseKeyboardHook.h"  


//共享内存变量
#pragma data_seg("MouseKeyboardHook")  
HHOOK g_hMouseHook = NULL;
HHOOK g_hKeyboardHook = NULL;
HWND g_lhDisplayWnd = NULL;
#pragma data_seg()  

int UnSetHook(unsigned int mouseKey = 3);
int SetHook(HWND main, unsigned int mouseKey = 3);

//钩子函数的格式 LRESULT CALLBACK 函数名(int 钩子类型, WPARAM wParam, LPARAM lParam);
//处理鼠标的钩子函数  
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
//处理键盘的钩子函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

//这个是获取DLL的内存地址，可以重复使用，当做模版  
HMODULE WINAPI ModuleFromAddress(PVOID pv);


LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	::SendMessage(g_lhDisplayWnd, WM_USER + 100, wParam, lParam);
	return ::CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	::SendMessage(g_lhDisplayWnd, WM_USER + 101, wParam, lParam);
	return ::CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

//安装钩子函数  
MOUSEKEYBOARDHOOK_API int SetHook(HWND main, unsigned int mouseKey)
{
	BOOL mouse = true;
	BOOL key = true;
	g_lhDisplayWnd = main;
	if (mouseKey & 0x1) {
		g_hMouseHook = ::SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc,
			ModuleFromAddress(MouseHookProc), 0);
		mouse = g_hMouseHook?1:0;
	}
	if (mouseKey & 0x2) {
		g_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc,
			ModuleFromAddress(KeyboardHookProc), 0);
		key = g_hKeyboardHook ? 1 : 0;
	}
	

	return mouse && key ? 1 : 0;
}

MOUSEKEYBOARDHOOK_API int UnSetHook(unsigned int mouseKey)
{
	BOOL mouse = true;
	BOOL key = true;
	g_lhDisplayWnd = NULL;
	if (mouseKey & 0x1) {
		mouse = ::UnhookWindowsHookEx(g_hMouseHook);
	}
	if (mouseKey & 0x2) {
		key = ::UnhookWindowsHookEx(g_hKeyboardHook);
	}

	return mouse && key ? 1 : 0;
}

HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
		return (HMODULE)mbi.AllocationBase;
	} else {
		return NULL;
	}
}