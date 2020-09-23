#include "hookutil.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <atlbase.h>
#include <UIAutomation.h>

#include <QSettings>
#include <QLibrary>
#include <QString>
#include <QTextCodec>
#include <QThread>
#include <QDir>
#include <QDebug>
#define DEBUG qDebug()<<__func__<<__LINE__

QString getWordAtPoint(POINT &pt,bool *ok);
bool HasDropHandler(const QString &ext);
BOOL ILIsFile(LPCITEMIDLIST pidl);
HWND g_lhDisplayWnd = NULL;
namespace HookUtil {
typedef unsigned int(*FUN1)(HWND hWnd,unsigned int hTypes);
typedef int(*FUN2)(unsigned int hTypes);
typedef void(*FUN4)(HWND);

static FUN1 SetHook = nullptr;
static FUN2 UnSetHook = nullptr;
static FUN4 SetHookMainHwnd = nullptr;
bool  GetFolderPath1(HWND hWnd, TCHAR *g_szPath);
#define CHECKFUN(fun) if (fun) {qDebug() << "load "#fun" ok!";}else {qDebug() << "load "#fun" err!";return false;}
bool loadHook(){
    QLibrary lib("HOOK.dll");
    if (lib.load()) {
        qDebug() << "load ok!";

        SetHook = FUN1(lib.resolve("SetHook"));
        UnSetHook = FUN2(lib.resolve("UnSetHook"));
        SetHookMainHwnd = FUN4(lib.resolve("SetHookMainHwnd"));
        CHECKFUN(SetHook);
        CHECKFUN(UnSetHook);
        CHECKFUN(SetHookMainHwnd);

        return true;
    } else {
        qDebug() << "load HOOK error!";
        return false;
    }
}
void setMainHwnd(HWND winId){
    if(!SetHookMainHwnd){
        qDebug() << "SetHookMainHwnd error!";
        return;
    }
    SetHookMainHwnd(winId);
}

unsigned int setHook(unsigned int hTypes, HWND hWnd){
    if(!SetHook){
        qDebug() << "SetHook error!";
        return false;
    }
    return SetHook(hWnd,hTypes);
}

unsigned int unSetHook(unsigned int hTypes){
    if(!UnSetHook){
        qDebug() << "UnSetHook error!";
        return false;
    }
    return UnSetHook(hTypes);
}

//----------------------------
#define SETHOOK(id,type,thread) if (!g_h##type && (hTypes&(1<< id))){\
printf("Sethook "#type" \n");\
g_h##type = SetWindowsHookEx(id, type##Proc, GetModuleHandle (NULL), thread);\
if(!g_h##type){ printf("Sethook "#type" err :%lu\n",GetLastError());} else{retMask|=(1<< id);}}

#define UNSETHOOK(id,hook) if (hook&& (hTypes&(1<< id))){printf("Unhook "#hook"\n");b = ::UnhookWindowsHookEx(hook);hook = NULL;\
if (!b) { printf("Unhook "#hook" err\n"); }else{retMask|=(1<< id);}}


HHOOK g_hLowLevelMouse = NULL;

//static bool isUpBtn = false;
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//    if (!isUpBtn && wParam == WM_LBUTTONUP) {
//        printf("LowLevelMouseProc WM_LBUTTONUP\n");
//    }
    if (g_lhDisplayWnd)
        ::SendMessage(g_lhDisplayWnd, WM_USER + 100 + WH_MOUSE_LL, wParam, lParam);

    return ::CallNextHookEx(g_hLowLevelMouse, nCode, wParam, lParam);
}

void setMainHwnd0(HWND winId){
    g_lhDisplayWnd = winId;
}

//安装钩子函数
unsigned int setHook0(unsigned int hTypes,HWND hWnd)
{
    DWORD   dwThread = 0;
    unsigned int retMask = 0;
    //只能作用于全局的钩子, dwThread需要是NULL，比如WH_MOUSE_LL、WH_KEYBOARD_LL
    if (hWnd) {
        dwThread = GetWindowThreadProcessId(hWnd, NULL);//g_ExpWnd
    }
    //SETHOOK(WH_MOUSE, Mouse, dwThread);
    SETHOOK(WH_MOUSE_LL, LowLevelMouse, dwThread)
/*
    if (!g_hLowLevelMouse && (hTypes&(1<< WH_MOUSE_LL))){
        printf("Sethook LowLevelMouse \n");
        g_hLowLevelMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle (NULL), 0);
        if(!g_hLowLevelMouse){
            DWORD err =  GetLastError();
            printf("Sethook LowLevelMouse err :%ul\n",err);
        } else{
            retMask|=(1<< WH_MOUSE_LL);}
    }
*/
    return retMask;
}

unsigned int unSetHook0(unsigned int hTypes)
{
    unsigned int retMask = 0;
    BOOL b = 1;
    UNSETHOOK(WH_MOUSE_LL,g_hLowLevelMouse);

    return retMask;
}


//--------------------------------


// hook class member function
LPVOID HookUtil::HookMethod(_In_ LPVOID lpVirtualTable, _In_ PVOID pHookMethod,
    _In_opt_ uintptr_t dwOffset)
{
    uintptr_t dwVTable	= *((uintptr_t*)lpVirtualTable);
    uintptr_t dwEntry	= dwVTable + dwOffset;
    uintptr_t dwOrig	= *((uintptr_t*)dwEntry);

    DWORD dwOldProtection;
    ::VirtualProtect((LPVOID)dwEntry, sizeof(dwEntry),
        PAGE_EXECUTE_READWRITE, &dwOldProtection);

    *((uintptr_t*)dwEntry) = (uintptr_t)pHookMethod;

    ::VirtualProtect((LPVOID)dwEntry, sizeof(dwEntry),
        dwOldProtection, &dwOldProtection);

    return (LPVOID) dwOrig;
}

}
