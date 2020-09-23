//#define _CRT_SECURE_NO_DEPRECATE ;//unsafe functions
#define _AFXDLL
#include <afxmt.h>
#include <Windows.h>
#include <ShlObj.h>
#include <WinUser.h>
#include <shellapi.h>

#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QLibrary>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <Shlwapi.h>
namespace HookUtil {
typedef int(*FUN1)(HWND);
typedef int(*FUN2)();
typedef void(*FUN3)(BOOL);
typedef void(*FUN4)(HWND);

static FUN1 SetHook = nullptr;
static FUN2 UnSetHook = nullptr;
#define CHECKFUN(fun) if (fun) {qDebug() << "load "#fun" ok!";}else {qDebug() << "load "#fun" err!";return false;}
QLibrary lib("HOOK.dll");

bool loadHook(const QString &name = QString("HOOK.dll")){
    bool ok = 0;
    if(lib.isLoaded()){
        if(!lib.unload()){
            qDebug() << "unload ok!";
            SetHook = nullptr;UnSetHook = nullptr;
            return false;
        }
    }
    if(!lib.isLibrary(name)){
        qDebug() << "not a Library";
        SetHook = nullptr;UnSetHook = nullptr;
        return false;
    }
    lib.setFileName(name);
    //QLibrary lib("HOOK.dll");
    if (lib.load()) {
        qDebug() << "load ok!";

        SetHook = FUN1(lib.resolve("SetHook"));
        UnSetHook = FUN2(lib.resolve("UnSetHook"));
        CHECKFUN(SetHook);
        CHECKFUN(UnSetHook);

        return true;
    } else {
        qDebug() << "load error!";
        return false;
    }
}

int setMouseHook(HWND winId){
    if(!SetHook){
        qDebug() << "SetHook error!";
        return false;
    }
    return SetHook(winId);
}
int unSetMouseHook(){
    if(!UnSetHook){
        qDebug() << "UnSetHook error!";
        return false;
    }
    return UnSetHook();
}

}

BOOL FSendDropFile(CString strFilePath, HWND hwnd);
void Inject(DWORD pid);

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setAcceptDrops(true);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btnLoadHook_clicked()
{
    HookUtil::loadHook(ui->lEdllPath->text());
}


void Dialog::on_btnSetHook_clicked()
{
    bool ok = true;
    quint64 aa =  ui->lEditHwnd->text().trimmed().toULongLong(&ok,16);
    if(!ok && !aa){
        aa = this->winId();
    }
    qDebug() <<QString("SetHook hwnd:%1").arg(aa,0,16);

    HookUtil::setMouseHook(HWND(aa));
    if(aa){
        //SetParent(HWND(this->winId()),HWND(aa));
        //SetWindowPos(HWND(this->winId()),HWND(aa),0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
        //::DragAcceptFiles(HWND(aa),FALSE);
        //Inject(aa);
    }
    //Inject(9412);
    //FSendDropFile(CString(_T("D:\\35.zip")), HWND(aa));
        //::EnableWindow(HWND(aa),FALSE);
}

void Dialog::on_btnUnSetHook_clicked()
{
    HookUtil::unSetMouseHook();
}

void Dialog::on_btnSetHookSelf_clicked()
{
    SetWindowPos(HWND(this->winId()),HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
    //HookUtil::setMouseHook(HWND(this->winId()));
}



//模拟拖拽消息

BOOL FSendDropFile(CString strFilePath, HWND hwnd)
{
    int                  nResult = FALSE;
    HWND                 hMain = nullptr;
    char                 szFile[MAX_PATH] = { 0 };
    DWORD                dwBufSize = 0;//sizeof(DROPFILES)   +   sizeof(szFile)   +   1;
    BYTE                 *pBuf = nullptr;
    DWORD                dwProcessId = 0;

    HANDLE               hProcess = 0;

    LPSTR                pszRemote = nullptr;

    wcstombs(szFile, strFilePath.GetBuffer(0), _MAX_PATH);

    dwBufSize = sizeof(DROPFILES) + strlen(szFile) + 1;

    hMain = hwnd? hwnd: (::FindWindow(nullptr, _T( "新建文件夹")));
    if (hMain == nullptr)
    {
        OutputDebugString(_T("不能找到加壳主程序!"));
        goto Exit0;
    }

    pBuf = new   BYTE[dwBufSize];
    if (pBuf == nullptr)
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
    pszRemote = (LPSTR)VirtualAllocEx(hProcess, nullptr, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

    if (nullptr == pszRemote)
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
        pBuf = nullptr;
    }
    return nResult;
}

void Inject(DWORD pid)
{
        CString strPID;
        CString str = "D:\\dlltest.dll";
        HMODULE hMod = NULL;
        LPTHREAD_START_ROUTINE lpThread = NULL;

        //获取PID
        //GetDlgItemText(IDC_EDIT_PID,strPID);
        //int nPid = _ttoi(strPID);
        //获取目标进程句柄
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
        //在目标进程内存中分配DllName(DLL文件绝对路径)大小的内存
        DWORD dwSize = (DWORD)(str.GetLength() + 1) * sizeof(TCHAR);
        LPVOID pRemoteBuf = VirtualAllocEx(hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE);
        //将MyDll路径写入目标进程内存
        WriteProcessMemory(hProcess,pRemoteBuf,(LPVOID)str.GetBuffer(0),dwSize,NULL);
        //获取LoadLibrary()地址
        hMod = GetModuleHandle(_T("kernel32.dll"));
        lpThread = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,"LoadLibraryW");
        //在目标进程里运行线程
        HANDLE hThread = CreateRemoteThread(hProcess,NULL,0,lpThread,pRemoteBuf,0,NULL);
        DWORD lerr = 0;
        if(!hThread)
        lerr = GetLastError();

        WaitForSingleObject(hThread,INFINITE);
        CloseHandle(hThread);
        CloseHandle(hProcess);
}
BOOL InjectDll(DWORD dwProcessID, const std::wstring &dllPath){//参数：目标进程ID、DLL路径
    FARPROC FuncAddr = NULL;
    HMODULE hdll = LoadLibrary(TEXT("Kernel32.dll"));//加载DLL
    if (hdll != NULL){
        FuncAddr = GetProcAddress(hdll, "LoadLibraryW");//获取LoadLibraryA函数地址
        if (FuncAddr == NULL)return FALSE;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwProcessID);//获取进程句柄
    if (hProcess == NULL)return FALSE;
    //CString strPath(dllPath);
    DWORD dwSize = dllPath.length()*sizeof (wchar_t) + 1;
    LPVOID RemoteBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);//远程申请内存
    SIZE_T dwRealSize;
    if (WriteProcessMemory(hProcess, RemoteBuf, (LPVOID)dllPath.c_str(), dwSize, &dwRealSize))//远程写内存
    {
        DWORD dwThreadId;
        HANDLE hRemoteThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)FuncAddr, RemoteBuf, 0, &dwThreadId);//创建远程线程
        if (hRemoteThread == NULL)
        {
            VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
            CloseHandle(hProcess);
            return FALSE;
        }
        //释放资源
        WaitForSingleObject(hRemoteThread, INFINITE);
        CloseHandle(hRemoteThread);
        VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
        CloseHandle(hProcess);
        return TRUE;
    }
    else
    {
        VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }
}
void Dialog::on_btnInjectDll_clicked()
{
    bool ok = true;
    quint32 pid =  ui->lEdpid->text().trimmed().toULong(&ok);
    QString path = ui->lEdllPath->text();
    if(!QFileInfo::exists(path) || !ok){
        QMessageBox::critical(this,"err",QString("pid:%1;path:%2").arg(pid).arg(path));
        return;
    }
    InjectDll(pid,QDir::toNativeSeparators(path).toStdWString());


}


void Dialog::on_btnSendCloseMsg_clicked()
{
    bool ok = true;
    quint64 aa =  ui->lEditHwnd->text().trimmed().toULongLong(&ok,16);
    if(ok && aa){
        LRESULT r = SendMessageA(HWND(aa),WM_CLOSE,0,0);
        qDebug() << QString("SendMessageA:%1").arg(r);
    }

}

void WorkerThread::run() {
    QMessageBox::information(nullptr,"aaa","bbb");
}

void Dialog::on_btnTest1_clicked()
{
    WorkerThread * work = new WorkerThread();
    work->start();
}
