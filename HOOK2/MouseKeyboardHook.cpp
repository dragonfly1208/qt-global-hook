#define _CRT_SECURE_NO_DEPRECATE ;//unsafe functions
#define _AFXDLL
#include <afxmt.h>
#include <Windows.h>
#include <Shlobj.h>
#include <afxwin.h>
#include "MouseKeyboardHook.h"  
#include "MyDropTarget.h"

//class CCriticalSection
//{
//	::CRITICAL_SECTION _object;
//public:
//	CCriticalSection() { InitializeCriticalSection(&_object); }
//	~CCriticalSection() { DeleteCriticalSection(&_object); }
//	void Enter() { EnterCriticalSection(&_object); }
//	void Leave() { LeaveCriticalSection(&_object); }
//};
//
//class CCriticalSectionLock
//{
//	CCriticalSection *_object;
//	void Unlock() { _object->Leave(); }
//public:
//	CCriticalSectionLock(CCriticalSection &object) : _object(&object) { _object->Enter(); }
//	~CCriticalSectionLock() { Unlock(); }
//};
//#define CS_LOCK CCriticalSectionLock lock(g_CS);
BOOL FSendDropFile(CString strFilePath, HWND hwnd);
BOOL SimulateDropFile(CString strFilePath, HWND hwnd);

//共享内存变量
#pragma data_seg("MouseKeyboardHook")  
//HHOOK g_hMouseHook = NULL;
HHOOK g_hKeyboardHook = NULL;
HWND g_lhDisplayWnd = NULL;
//CCriticalSection g_CS;
//CCriticalSectionLock *g_SLock = nullptr;

CMutex Section;
CSingleLock *g_SLock = nullptr;

bool g_Locked = false;

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

HHOOK g_hMouse = NULL;
HHOOK g_hLowLevelMouse = NULL;

MyDropTarget g_MyDrop;
#pragma data_seg()  
int indexLog = 0;


int UnSetHook(unsigned int mouseKey = 3);
int SetHook(HWND main, unsigned int mouseKey = 3);

//钩子函数的格式 LRESULT CALLBACK 函数名(int 钩子类型, WPARAM wParam, LPARAM lParam);
//处理鼠标的钩子函数  
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
//处理键盘的钩子函数
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

//这个是获取DLL的内存地址，可以重复使用，当做模版  
HMODULE WINAPI ModuleFromAddress(PVOID pv);


LRESULT CALLBACK SelfNoProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT hr = ::CallNextHookEx(g_hCallWnd, nCode, wParam, lParam);
	return 1;
}
LRESULT CALLBACK AllNoProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

//#define WM_DROPFILES		0x0233
LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_lhDisplayWnd && g_lhDisplayWnd) {
		return 1;
	}
	
	CWPSTRUCT *cwp = (CWPSTRUCT *)lParam;
	CString str;
	if (cwp) {

		//if (cwp->message == WM_NCDESTROY) {//WM_NCDESTROY                    0x0082
		//	OutputDebugString(_T("CallWndProc WM_NCDESTROY before Sleep.\n"));
		//	Sleep(5000);
		//	OutputDebugString(_T("CallWndProc WM_NCDESTROY after Sleep.\n"));
		//}
		switch (cwp->message)
		{
		case WM_PAINT://0x000F
		//case WM_DESTROY://2
		case WM_ERASEBKGND://                   0x0014
		case WM_SHOWWINDOW://18
		case WM_GETICON://7f
		case WM_SETCURSOR://20
		case WM_NOTIFY ://                      0x004E
		//case WM_NCHITTEST://84
		case WM_NCPAINT://                      0x0085
		//case WM_NCACTIVATE://                   0x0086
		//case WM_WINDOWPOSCHANGING://46
		case WM_WINDOWPOSCHANGED://47
		case WM_ACTIVATEAPP://1c
		//case WM_CHANGEUISTATE://                0x0127
		//case WM_UPDATEUISTATE://                0x0128
		case WM_PARENTNOTIFY://                 0x0210
		case WM_IME_SETCONTEXT://               0x0281
		case WM_PRINTCLIENT://                 0x0318

		case 0x0407:
		case 0x0418:
		case 0x043f:
		case 0x0440:
		//case 0x0D00:
			break;
		case WM_NCHITTEST://84
		{

			LRESULT hr = ::CallNextHookEx(g_hCallWnd, nCode, wParam, lParam);
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			str.Format(_T("[%04X] CallWndProc :nCode=%d HitTest,%X ,%X ;(%d,%d);%X;%X.\n"),indexLog++, nCode, (long)wParam, (long)lParam, xPos, yPos, hr, cwp->hwnd);
			OutputDebugString(str);
			return hr;//1
		}break;
		//case WM_COPYDATA://                     0x004A
		//{

		//}break;
		default:
			str.Format(_T("[%04X] CallWndProc :nCode=%d,%X ;%X ,%X ,%X ,%X .\n"), indexLog++, nCode, (long)wParam, cwp->message, cwp->lParam, cwp->wParam, cwp->hwnd);
			break;
		}
	}
	else {
		str.Format(_T("[%04X] CallWndProc :nCode=%d,%X ,%X .\n"), indexLog++, nCode, (long)wParam, (long)lParam);
	}
	OutputDebugString(str);
	return ::CallNextHookEx(g_hCallWnd, nCode, wParam, lParam);
	
}

LRESULT CALLBACK CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPRETSTRUCT  *cwp = (CWPRETSTRUCT  *)lParam;
	CString str;
	if (cwp) {
		switch (cwp->message)
		{
		//WM_DESTROY                      0x0002
		//case WM_NOTIFY://                      0x004E
		case WM_GETICON://7f
		case WM_SETCURSOR://20
			//WM_NCDESTROY                    0x0082
		case WM_NCHITTEST://84
		case WM_WINDOWPOSCHANGING://            0x0046
//WM_WINDOWPOSCHANGED             0x0047
//WM_NCCREATE                     0x0081
//WM_NCDESTROY                    0x0082
//SPI_GETDOCKMOVING               0x0090
//SPI_SETDOCKMOVING               0x0091
#define WM_CAPTURECHANGED               0x0215
		case 0x0402:
		case 0x0407:
		case 0x0418:
		case 0x04D1:
		case 0x04D2:
		case 0x043f:
		case 0x0440:
			break;
		default:
			str.Format(_T("[%04X] CallWndRetProc :nCode=%d,%X ;%X ,%X ;%X ,%X ;%X .\n"), indexLog++, nCode, (long)wParam, cwp->message, cwp->lResult, cwp->lParam, cwp->wParam, cwp->hwnd);
			break;
		}
	}
	else {
		str.Format(_T("[%04X] CallWndRetProc :nCode=%d,%X ,%X .\n"), indexLog++, nCode, (long)wParam, (long)lParam);
	}
	OutputDebugString(str);
	return ::CallNextHookEx(g_hCallWndRet, nCode, wParam, lParam);
}
#define CONSTR(co) (co?co:CString())
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CString str;
	switch (nCode)
	{
	case HCBT_CLICKSKIPPED://6
	{
		switch (wParam) {
		case WM_NCMOUSEMOVE://A0
		case WM_MOUSEMOVE://200
			return ::CallNextHookEx(g_hCBT, nCode, wParam, lParam);
		default:
			break;
		}

		MOUSEHOOKSTRUCT *mhs = (MOUSEHOOKSTRUCT *)lParam;
		if (mhs) {
			switch (mhs->dwExtraInfo) {
			case 0xFFFF:
				break;
			default:
				str.Format(_T("[%04X] CBTProc :nCode=%d click skipped,%X ;%X, %d.(%d,%d)\n"),indexLog++, nCode, (long)wParam, mhs->hwnd, mhs->wHitTestCode, mhs->pt.x, mhs->pt.y);
				break;
			}
		}
	

	}break;
	case HCBT_CREATEWND:
	{
		CBT_CREATEWNDA  *mhs = (CBT_CREATEWNDA  *)lParam;
		if (mhs) {
			if (mhs->lpcs) {
				LPCREATESTRUCTA lpcs = mhs->lpcs;
				str.Format(_T("[%04X] CBTProc :nCode=%d creat wind,%X ;%X,%s ,%s\n"),indexLog++, nCode, (long)wParam, lpcs->hwndParent, CONSTR(lpcs->lpszName), CONSTR(lpcs->lpszClass));
			}else {
				str.Format(_T("[%04X] CBTProc :nCode=%d creat wind,%X ;%X,%X \n"),indexLog++, nCode, (long)wParam, 0, mhs->hwndInsertAfter);
			}
			
		}
	}break;
	case HCBT_DESTROYWND:
		break;
	default:
		str.Format(_T("[%04X] CBTProc :nCode=%d,%X ,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
		break;
	}
	OutputDebugString(str);
	return ::CallNextHookEx(g_hCBT, nCode, wParam, lParam);
}
LRESULT CALLBACK ForegroundIdleProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("[%04X] ForegroundIdleProc :nCode=%d,%X ,,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	OutputDebugString(str);
	return ::CallNextHookEx(g_hForegroundIdle, nCode, wParam, lParam);
}
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	
	MSG * msg = (MSG *)lParam;
	CString str;
	if (msg) {
		switch (msg->message)
		{
		case WM_NCMOUSEMOVE://A0
		case WM_TIMER://113
		case WM_MOUSEMOVE://200
		case DM_GETDEFID://(WM_USER + 0)400
		/*case WM_NCMOUSEMOVE://A0
		case WM_NCMOUSELEAVE://2A2
		//case WM_MOUSEHOVER://2A1
		case WM_MOUSELEAVE://2A3
		case WM_MOUSEMOVE://200
		case WM_TIMER://113
		case WM_POWERBROADCAST://218
		case WM_PAINT://F
			//WM_USER = 0X400
		case DM_GETDEFID://(WM_USER + 0)400
		case DM_SETDEFID://(WM_USER + 1)401
		case 0x04B5:*/
		case 0X8000:
			break;
		//case WM_DROPFILES:
		//case WM_MOUSEMOVE://200
		//{
		//	str.Format(_T("[%04X] GetMsgProc :nCode=%d,%X ;%X .\n"),indexLog++, nCode, (long)wParam, msg->message);
		//	OutputDebugString(str);
		//	return 1;
		//}
		default:
			str.Format(_T("[%04X] GetMsgProc :nCode=%d,%X ;%X .\n"),indexLog++, nCode, (long)wParam, msg->message);
			break;
		}
		//if (msg->message == 0x118) {
		//	OutputDebugString(_T("GetMsgProc before sleep 5s\n"));
		//	Sleep(5000);
		//	OutputDebugString(_T("GetMsgProc after sleep 5s\n"));
		//}
	}
	else {
		str.Format(_T("[%04X] GetMsgProc1 :nCode=%d,%X ,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	}
	if (wParam)
		OutputDebugString(str);
	
	return ::CallNextHookEx(g_hGetMsg, nCode, wParam, lParam);
}
LRESULT CALLBACK JournalPlaybackProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("[%04X] JournalPlaybackProc :nCode=%d,%X ,,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	OutputDebugString(str);
	return ::CallNextHookEx(g_hJournalPlayback, nCode, wParam, lParam);
}
LRESULT CALLBACK JournalRecordProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("[%04X] JournalRecordProc :nCode=%d,%X ,,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	OutputDebugString(str);
	return ::CallNextHookEx(g_hJournalRecord, nCode, wParam, lParam);
}
LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	OutputDebugString(_T("MessageProc :return 1.\n"));
	return 1;
	MSG * msg = (MSG *)lParam;
	CString str;
	if (msg) {
		switch (msg->message)
		{

		case WM_MOUSEMOVE://200
		case WM_TIMER://113
		//case WM_PAINT://F
					  //WM_USER = 0X400
		//case DM_GETDEFID://(WM_USER + 0)
		//case DM_SETDEFID://(WM_USER + 1)
			break;
		default:
			str.Format(_T("[%04X] MessageProc :nCode=%d,%X ;%X .\n"),indexLog++, nCode, (long)wParam, msg->message);
			break;
		}
	}else {
		str.Format(_T("[%04X] MessageProc :nCode=%d,%X ,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	}
	OutputDebugString(str);
	return ::CallNextHookEx(g_hMessage, nCode, wParam, lParam);
}
LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	HSHELL_APPCOMMAND;
	CString str;
	str.Format(_T("[%04X] ShellProc :nCode=%d,%X ,,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	OutputDebugString(str);
	return ::CallNextHookEx(g_hShell, nCode, wParam, lParam);
}
LRESULT CALLBACK SysMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG * msg = (MSG *)lParam;
	CString str;
	if (msg) {
		switch (msg->message)
		{

		case WM_MOUSEMOVE://200
		//case WM_TIMER://113
		case WM_PAINT://F
					  //WM_USER = 0X400
		//case DM_GETDEFID://(WM_USER + 0)
		//case DM_SETDEFID://(WM_USER + 1)
			break;
		default:
			str.Format(_T("[%04X] SysMsgProc :nCode=%d :%X .\n"),indexLog++, nCode,  msg->message);
			break;
		}
	}
	else {
		str.Format(_T("[%04X] SysMsgProc :nCode=%d:%X \n"),indexLog++, nCode,  (long)lParam);
	}
	OutputDebugString(str);
	//str.Format(_T("[%04X] SysMsgProc :nCode=%d,%X ,%X .\n"),indexLog++, nCode, (long)wParam, (long)lParam);
	return ::CallNextHookEx(g_hSysMsg, nCode, wParam, lParam);
}


static BOOL mu= FALSE;
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CString str;
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

	return ::CallNextHookEx(g_hMouse, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	
	MSLLHOOKSTRUCT * msll = (MSLLHOOKSTRUCT *)lParam;
	CString str;
	if (msll) {
		CString wp;
		switch (wParam)
		{
		case WM_LBUTTONDOWN:wp = _T("LD"); break;
		case WM_LBUTTONUP:wp = _T("LU"); break;
		//case WM_MOUSEMOVE:wp = _T("MM"); break;
		case WM_MOUSEWHEEL:wp = _T("MW"); break;
		case WM_MOUSEHWHEEL:wp = _T("MHW"); break;
		case WM_RBUTTONDOWN:wp = _T("RD"); break;
		case WM_RBUTTONUP:wp = _T("RU"); break;
		default:
			return ::CallNextHookEx(g_hLowLevelMouse, nCode, wParam, lParam);
		break;
		}
		str.Format(_T("[%04X] LowLevelMouseProc :nCode=%d,%d %s :(%d,%d) .\n"),indexLog++, nCode, wParam,wp.GetBuffer(), msll->pt.x, msll->pt.y);
	}
	else {
		str.Format(_T("[%04X] LowLevelMouseProc :nCode=%d:%X \n"),indexLog++, nCode, (long)lParam);
	}
	OutputDebugString(str);

	//if (wParam != WM_MOUSEMOVE) {
	//	CString str;
	//	str.Format(_T("[%04X] MouseHookProc :nCode=%d,%X.\n"),indexLog++, nCode, (long)wParam);
	//	OutputDebugString(str);
	//}
	//if (wParam == WM_LBUTTONUP) {
	//	OutputDebugString(_T("before SendMessage.\n"));
	//	if (g_lhDisplayWnd)::SendMessage(g_lhDisplayWnd, WM_USER + 100, wParam, lParam);
	//	OutputDebugString(_T("after SendMessage.\n"));
	//}
	
	return ::CallNextHookEx(g_hLowLevelMouse, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(g_lhDisplayWnd)::SendMessage(g_lhDisplayWnd, WM_USER + 101, wParam, lParam);
	return ::CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}


BOOL CallChangeWindowMessageFilter(HWND hWnd, UINT nMsg, DWORD dwAction, PCHANGEFILTERSTRUCT chfit)
{
	typedef BOOL(WINAPI *LPFUNC)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);

	HMODULE hLib = LoadLibraryA("user32.dll");

	if (hLib)
	{
		LPFUNC func = (LPFUNC)GetProcAddress(hLib, "ChangeWindowMessageFilterEx");

		if (func && chfit)
			return func(hWnd, nMsg, dwAction, chfit);
	}

	return FALSE;
}



#define SETHOOK(id,type,thread) if (!g_h##type){\
OutputDebugString(_T("Sethook "#type" \n"));\
g_h##type = SetWindowsHookEx(id, type##Proc, ModuleFromAddress(type##Proc), thread);\
if(!g_h##type){ OutputDebugString(_T("Sethook "#type" err \n"));}}

#define SETHOOK2(id,type,thread,func) if (!g_h##type){\
OutputDebugString(_T("Sethook "#type" \n"));\
g_h##type = SetWindowsHookEx(id, func, ModuleFromAddress(func), thread);\
if(!g_h##type){ OutputDebugString(_T("Sethook "#type" err \n"));}}

//安装钩子函数  
MOUSEKEYBOARDHOOK_API int SetHook(HWND main)
{
	HRESULT hr = 0;
	BOOL mouse = true;
	BOOL key = true;
	HWND hWnd = 0;
	DWORD   dwThread = 0;
	if (main) {
		g_lhDisplayWnd = main;
		hWnd = main;
	}
	if (!hWnd) {
		hWnd = GetShellWindow();
		//hWnd = HWND(0X00660EF2);// (0X00010B20);
	}
	//hWnd = HWND(0X001409F4);// (0X00010B20);
	//BOOL ook = g_MyDrop.RegisterDropTarget(hWnd);
	if (hWnd) {
		//::EnableWindow(hWnd, FALSE);
		//FSendDropFile(_T("D:\\35.zip"), 0);
		//SimulateDropFile(_T("D:\\35.zip"), hWnd);
		dwThread = GetWindowThreadProcessId(hWnd, NULL);//g_ExpWnd
		//return 1;
	}
	//else {
	//	FSendDropFile(_T("D:\\35.zip"), 0);
	//	return 0;
	//}
		
		//g_hMouse = ::SetWindowsHookEx(WH_MOUSE, MouseProc,
		//	ModuleFromAddress(MouseProc), dwThread);
		//if (!g_hMouse)
		//	return 0;
		BOOL b1 = ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		BOOL b2 = ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);       // 0x0049 == WM_COPYGLOBALDATA
		BOOL b3 = ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		
		//CHANGEFILTERSTRUCT chfit = { sizeof(CHANGEFILTERSTRUCT) };

		///BOOL b4  = CallChangeWindowMessageFilter(hWnd, WM_DROPFILES, MSGFLT_ALLOW, &chfit);
		//BOOL b5  = CallChangeWindowMessageFilter(hWnd, 0x0049, MSGFLT_ALLOW, &chfit);       // 0x0049 == WM_COPYGLOBALDATA
		//BOOL b6 = CallChangeWindowMessageFilter(hWnd, WM_COPYDATA, MSGFLT_ALLOW, &chfit);
		/*
		CHANGEFILTERSTRUCT chfit = { sizeof(CHANGEFILTERSTRUCT) };
		BOOL b = ChangeWindowMessageFilterEx(hWnd, WM_COPYDATA, MSGFLT_ALLOW, &chfit);
		DWORD lsterr = 0;
		if (!b)
			lsterr = GetLastError();
		*/
		//BOOL b4  = ChangeWindowMessageFilterEx(hWnd, WM_DROPFILES, MSGFLT_ALLOW, NULL);
		//BOOL b5  = ChangeWindowMessageFilterEx(hWnd, 0x0049, MSGFLT_ALLOW, NULL);       // 0x0049 == WM_COPYGLOBALDATA
		//BOOL b6 = ChangeWindowMessageFilterEx(hWnd, WM_COPYDATA, MSGFLT_ALLOW, NULL);
		
		/*g_ShellHook = ::SetWindowsHookEx(WH_SHELL, ShellHookProc,
			ModuleFromAddress(ShellHookProc), dwThreadID);*/
		//if(!g_CBTHook)
		//g_CBTHook = ::SetWindowsHookEx(WH_CBT, CBTHookProc,
		//	ModuleFromAddress(CBTHookProc), 0);
		//if (!g_CBTHook)
		//	return 0;
		//if(!g_ShellHook)
		//g_ShellHook = ::SetWindowsHookEx(WH_SHELL, ShellHookProc,
		//	ModuleFromAddress(ShellHookProc), 0);
		//if (!g_ShellHook)
		//	return 0;

		//if (!g_hShell){
		//	OutputDebugString(_T("Sethook Shell \n"));
		//	g_hShell = SetWindowsHookEx(WH_SHELL, ShellProc, ModuleFromAddress(ShellProc), dwThread);
		//}

		//SETHOOK(WH_MSGFILTER, Message, 0);
		//SETHOOK(WH_GETMESSAGE, GetMsg, 0);
		//SETHOOK(WH_MOUSE, Mouse, 0);//无用
		//SETHOOK(WH_CALLWNDPROC, CallWnd, 0);
		//SETHOOK(WH_CALLWNDPROCRET, CallWndRet, 0);
		//SETHOOK(WH_CBT, CBT, 0);

		//SETHOOK(WH_MOUSE_LL, LowLevelMouse, 0);//无用
		//SETHOOK(WH_SHELL, Shell, 0);

		//SETHOOK(WH_SYSMSGFILTER, SysMsg, 0);//无用


		//SETHOOK(WH_CALLWNDPROC, CallWnd, dwThread);
		//SETHOOK(WH_CALLWNDPROCRET, CallWndRet, dwThread);
		//SETHOOK(WH_CBT, CBT, dwThread);
		//SETHOOK(WH_FOREGROUNDIDLE, ForegroundIdle, dwThread);
		//SETHOOK(WH_GETMESSAGE, GetMsg, dwThread);
		////SETHOOK(WH_JOURNALPLAYBACK, JournalPlayback, dwThread);
		////SETHOOK(WH_JOURNALRECORD, JournalRecord, dwThread);
		SETHOOK(WH_MSGFILTER, Message, dwThread);
		//SETHOOK(WH_SHELL, Shell, dwThread);

		//SETHOOK(WH_MOUSE, Mouse, dwThread);

		//SETHOOK2(WH_FOREGROUNDIDLE, ForegroundIdle, dwThread, SelfNoProc);
		//SETHOOK2(WH_CALLWNDPROCRET, CallWndRet, dwThread, SelfNoProc);
		//SETHOOK2(WH_CBT, CBT, dwThread, SelfNoProc);
		//SETHOOK2(WH_MSGFILTER, Message, dwThread, SelfNoProc);
		//SETHOOK2(WH_SHELL, Shell, dwThread, SelfNoProc);
		//SETHOOK2(WH_MOUSE, Mouse, dwThread, SelfNoProc);
		//SETHOOK2(WH_CALLWNDPROC, GetMsg, dwThread, AllNoProc);
		//SETHOOK2(WH_GETMESSAGE, GetMsg, dwThread, AllNoProc);

		//SelfNoProc AllNoProc

		

		//	HWND hProgman = GetShellWindow();


		//if (!g_hMessage) {
		//	OutputDebugString(_T("SetUnhook WH_GETMESSAGE \n"));
		//	g_hMessage = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, GetModuleHandle(_T("HOOK.dll")), dwThreadID);
		//}
		
	

	return 1;
}
#define UNSETHOOK(hook) if (hook){OutputDebugString(_T("Unhook "#hook"\n"));b = ::UnhookWindowsHookEx(hook);hook = NULL;if (!b) { OutputDebugString(_T("Unhook "#hook" err\n")); return 0; }}
MOUSEKEYBOARDHOOK_API int UnSetHook()
{

	g_lhDisplayWnd = NULL;
	BOOL b = 1;

	//UNSETHOOK(g_hMouseHook);
	//if (g_hMouseHook) {
	//	b = ::UnhookWindowsHookEx(g_hMouseHook);
	//	g_hMouseHook = NULL;
	//	if (!b) return 0;
	//}
	UNSETHOOK(g_hCallWnd);
	UNSETHOOK(g_hCallWndRet);
	UNSETHOOK(g_hCBT);
	UNSETHOOK(g_hForegroundIdle);
	UNSETHOOK(g_hGetMsg);
	UNSETHOOK(g_hJournalPlayback);
	UNSETHOOK(g_hJournalRecord);
	UNSETHOOK(g_hMessage);
	UNSETHOOK(g_hShell);

	UNSETHOOK(g_hMouse);
	UNSETHOOK(g_hLowLevelMouse);
	
		
		
	return b;
}
HINSTANCE hInstanceHandle = NULL;   //DLL模块句柄

HMODULE WINAPI ModuleFromAddress(PVOID pv)
{
	if (!hInstanceHandle) {
		//hInstanceHandle = AfxGetInstanceHandle();
		hInstanceHandle = GetModuleHandle(_T("HOOK.dll"));
	}
	return hInstanceHandle;
	/*
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) {
		return (HMODULE)mbi.AllocationBase;
	} else {
		return NULL;
	}
	*/
}





/*
CallWnd
CallWndRet
CBT
ForegroundIdle
GetMsg
JournalPlayback
JournalRecord
Message
Shell
SysMsg


*/


//模拟拖拽消息

BOOL FSendDropFile(CString strFilePath, HWND hwnd)
{
	int                  nResult = FALSE;
	HWND                 hMain = NULL;
	char                 szFile[MAX_PATH] = { 0 };
	DWORD                dwBufSize = 0;//sizeof(DROPFILES)   +   sizeof(szFile)   +   1;  
	BYTE                 *pBuf = NULL;
	DWORD                dwProcessId = 0;

	HANDLE               hProcess = 0;

	LPSTR                pszRemote = NULL;

	wcstombs(szFile, strFilePath.GetBuffer(0), _MAX_PATH);

	dwBufSize = sizeof(DROPFILES) + strlen(szFile) + 1;
	
	hMain = hwnd? hwnd: (::FindWindow(NULL, _T("新建文件夹")));
	if (hMain == NULL)
	{
		OutputDebugString(_T("不能找到加壳主程序!"));
		goto Exit0;
	}

	pBuf = new   BYTE[dwBufSize];
	if (pBuf == NULL)
		goto Exit0;

	memset(pBuf, 0, dwBufSize);
	DROPFILES   *pDrop = (DROPFILES   *)pBuf;
	pDrop->pFiles = sizeof(DROPFILES);
	strcpy((char   *)(pBuf + sizeof(DROPFILES)), szFile);

	GetWindowThreadProcessId(hMain, &dwProcessId);
	if (0 == dwProcessId)
	{
		goto Exit0;
	}
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessId);
	if (hProcess == 0)
	{
		goto Exit0;
	}
	pszRemote = (LPSTR)VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	if (NULL == pszRemote)
	{
		goto Exit0;
	}

	if (WriteProcessMemory(hProcess, pszRemote, pBuf, dwBufSize, 0))
	{
		::SendMessage(hMain, WM_DROPFILES, (WPARAM)pszRemote, NULL);
	}
	else
	{
		goto Exit0;
	}
	nResult = TRUE;
Exit0:
	if (pBuf)
	{
		delete[] pBuf;
		pBuf = NULL;
	}
	return nResult;
}

BOOL SimulateDropFile(CString strFilePath, HWND hwnd)
{
	char szFile[MAX_PATH] = { 0 };
	wcstombs(szFile, strFilePath.GetBuffer(0), _MAX_PATH);
	DWORD dwBufSize = sizeof(DROPFILES) + strlen(szFile) + 1;

	//通过类名或窗口标题 找到接受拖拽的窗口
	HWND hMain = hwnd? hwnd: (::FindWindow(NULL, _T("新建文件夹")));
	if (hMain == NULL)
		return FALSE;
	BYTE* pBuf = new BYTE[dwBufSize];
	if (pBuf == NULL)
		return FALSE;

	BOOL bResult = FALSE;
	memset(pBuf, 0, dwBufSize);
	DROPFILES* pDrop = (DROPFILES*)pBuf;
	pDrop->pFiles = sizeof(DROPFILES);
	strcpy((char*)(pBuf + sizeof(DROPFILES)), szFile);

	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hMain, &dwProcessId);
	if (dwProcessId != NULL)
	{
		HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessId);
		if (hProcess != NULL)
		{
			LPSTR pszRemote = (LPSTR)VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
			if (pszRemote && WriteProcessMemory(hProcess, pszRemote, pBuf, dwBufSize, 0))
			{
				::SendMessage(hMain, WM_DROPFILES, (WPARAM)pszRemote, NULL);
				bResult = TRUE;
			}
		}
	}

	if (pBuf)
	{
		delete[] pBuf;
		pBuf = NULL;
	}
	return bResult;
}
