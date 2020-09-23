#ifndef MOUSEKEYBOARDHOOK_H
#define MOUSEKEYBOARDHOOK_H


#ifdef HOOK_EXPORTS
#define MOUSEKEYBOARDHOOK_API __declspec(dllexport)  
#else
#define MOUSEKEYBOARDHOOK_API __declspec(dllimport)  
#endif


#include <Windows.h>

//安装钩子函数  
extern "C" MOUSEKEYBOARDHOOK_API unsigned int SetHook(HWND hWnd, unsigned int hTypes);
extern "C" MOUSEKEYBOARDHOOK_API unsigned int UnSetHook(unsigned int hTypes);
extern "C" MOUSEKEYBOARDHOOK_API void SetHookMainHwnd(HWND hwnd);

#endif //MOUSEKEYBOARDHOOK