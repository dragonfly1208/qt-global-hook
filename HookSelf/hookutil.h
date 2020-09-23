#ifndef HOOKUTIL_H
#define HOOKUTIL_H

//#ifdef Q_OS_WIN
//#include <windef.h>
#include <Windows.h>
#include <QThread>
class QString;
class IShellWindows;
namespace HookUtil {
enum HookType{
    JOURNALRECORD     = 1<<0
    ,JOURNALPLAYBACK  = 1<<1
    ,KEYBOARD         = 1<<2
    ,GETMESSAGE       = 1<<3
    ,CALLWNDPROC      = 1<<4
    ,CBT              = 1<<5
    ,SYSMSGFILTER     = 1<<6
    ,MOUSE            = 1<<7
    ,HARDWARE         = 1<<8
    ,DEBUG            = 1<<9
    ,SHELL            = 1<<10
    ,FOREGROUNDIDLE   = 1<<11
    ,CALLWNDPROCRET   = 1<<12
    ,KEYBOARD_LL      = 1<<13
    ,MOUSE_LL         = 1<<14
};
//解决方案1：赖于DLL,将挂钩方法放在 dll中
    bool loadHook();
    void setMainHwnd(HWND winId);
    unsigned int setHook(unsigned int hTypes, HWND hWnd = nullptr);
    unsigned int unSetHook(unsigned int hTypes);
/*---------------------------------*/
    //解决方案1：将挂钩方法放在 本程序中。
    //只有四种:WH_JOURNALPLAYBACK，WH_JOURNALRECORD，WH_KEYBOARD_LL，WH_MOUSE_LL
    //https://blog.csdn.net/weixin_30487317/article/details/96691548
    void setMainHwnd0(HWND winId);
    unsigned int setHook0(unsigned int hTypes, HWND hWnd = nullptr);
    unsigned int unSetHook0(unsigned int hTypes);



    LPVOID HookMethod(_In_ LPVOID lpVirtualTable, _In_ PVOID pHookMethod,
        _In_opt_ uintptr_t dwOffset);

}




#endif // HOOKUTIL_H
