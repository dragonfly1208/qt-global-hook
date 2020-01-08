#include "widget.h"
#include "ui_widget.h"

#include <QScrollBar>
#include <QTextCodec>

#ifdef Q_OS_WIN
//#include <windef.h>
//#include <Windows.h>
#include <ShlObj.h>
#include <QLibrary>

typedef int(*FUN1)(HWND,unsigned int mouseKey);
typedef int(*FUN2)(unsigned int mouseKey);
#endif

#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << (setGlobalHook(true) ? "set ok" : "set failed");

    connect(this, &Widget::mousekeyboardAction,
            [this](){
        static int i = 0;
        QString debug = QString::number(i++) + "\tmouse_keyboard_action\n";
        qDebug() << debug;
        //ui->textBrowser->insertPlainText(debug);
    });
    connect(ui->textBrowser->verticalScrollBar(), &QScrollBar::rangeChanged,
            [this](int min, int max) {
        ui->textBrowser->verticalScrollBar()->setValue(max);
    });
}

Widget::~Widget()
{
    qDebug() << (setGlobalHook(false) ? "unset ok" : "unset failed");

    delete ui;
}

bool Widget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        switch (msg->message) {

        case WM_USER + 100:
            //emit mousekeyboardAction();
            //qDebug() << "mouse_action"<<msg->pt.x<<msg->pt.y;
            if(msg->wParam == WM_LBUTTONDBLCLK){
                qDebug() << "WM_LBUTTONDBLCLK"<<msg->pt.x<<msg->pt.y;
            }else if(msg->wParam == WM_LBUTTONDOWN){
                qDebug() << "WM_LBUTTONDOWN"<<msg->pt.x<<msg->pt.y;
            }else if(msg->wParam == WM_LBUTTONUP){
                //qDebug() << "WM_LBUTTONUP"<<msg->pt.x<<msg->pt.y;
                QString path = getExplorerPath(msg->pt);

                qDebug() << "WM_LBUTTONUP"<<msg->pt.x<<msg->pt.y<<path;

            }
            break;
        case WM_USER + 101:
            //emit mousekeyboardAction();
            qDebug() << "keyboard_action";
            break;
        default:
            break;
        }
    }
#endif
    return false;
}

bool Widget::setGlobalHook(bool b)
{
#ifdef Q_OS_WIN
    static FUN1 SetHook = NULL;
    static FUN2 UnSetHook = NULL;
    if (b) {
        QLibrary lib("HOOK.dll");
        if (lib.load()) {
            qDebug() << "load ok!";

            SetHook = (FUN1)lib.resolve("SetHook");
            UnSetHook = (FUN2)lib.resolve("UnSetHook");
            if (SetHook) {
                qDebug() << "load SetHook ok!";
                SetHook((HWND)this->winId(),3);
            }
            if (UnSetHook) {
                qDebug() << "load UnSetHook ok!";
            }
            return true;
        } else {
            qDebug() << "load error!";
            return false;
        }
    } else {
        if (UnSetHook)
            return UnSetHook(3);
        else
            return false;
    }
#endif
}

QString Widget::getExplorerPath(POINT Point){
    CHAR	szClass[100];
    HWND	hChooseHandle = ::WindowFromPoint(Point);
    HWND	hParent = ::GetParent(hChooseHandle);
    CHAR path[255];
    // 得到当前鼠标所在的窗口，判断是桌面还是资源管理器

    QString retPath;
    GetClassNameA(hParent, szClass, 100);
    if (_stricmp(szClass, "SHELLDLL_DefView") == 0)
    {
        hParent = ::GetParent(hParent);
        GetClassNameA(hParent, szClass, 100);
        if (_stricmp(szClass, "WorkerW") == 0)
        {
            SHGetSpecialFolderPathA(0, path, CSIDL_DESKTOPDIRECTORY, 0);
            retPath =  QTextCodec::codecForLocale()->toUnicode(path);
            //MessageBoxA(NULL, path, NULL, MB_OK);
        }
        else
        {
            hParent = hChooseHandle;
            for (int i = 0; i < 6; i++)
            {
                hParent = ::GetParent(hParent);
            }
            const char* szBuffer[6] = { "WorkerW","ReBarWindow32","Address Band Root","msctls_progress32","Breadcrumb Parent","ToolbarWindow32" };
            for (int i = 0; i < 6; i++)
            {
                HWND	hChild = ::GetWindow(hParent, GW_CHILD);
                while (hChild != NULL)
                {
                    GetClassNameA(hChild, szClass, 100);
                    if (_stricmp(szClass, szBuffer[i]) == 0)
                    {
                        hParent = hChild;
                        break;
                    }
                    hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
                }
            }
            ::GetWindowTextA(hParent, path, 100);//得到资源管理器所代表的文件夹路径
            //MessageBoxA(NULL, path + strlen("路径："), NULL, MB_OK);
            retPath =  QTextCodec::codecForLocale()->toUnicode(path);

            //return QString::fromLocal8Bit(path);
        }

    }
    else
    {
        GetClassNameA(hChooseHandle, szClass, 100);
        if (_stricmp(szClass, "SHELLDLL_DefView") == 0)
        {
            SHGetSpecialFolderPathA(0, path, CSIDL_DESKTOPDIRECTORY, 0);
            //MessageBoxA(NULL, path, NULL, MB_OK);
            retPath = QTextCodec::codecForLocale()->toUnicode(path);
        }
        else
        {

        }
    }
    static int start = -1;
    if(!retPath.isEmpty()){
        if(start<0){
            start = retPath.indexOf(':')+2;
        }
        return retPath.mid(start);
    }
    return retPath;
}
