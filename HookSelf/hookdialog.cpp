#include "hookdialog.h"
#include "ui_hookdialog.h"
#include "hookutil.h"

#include <QDebug>
HookDialog::HookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HookDialog)
{
    ui->setupUi(this);
}

HookDialog::~HookDialog()
{
    delete ui;
}

void HookDialog::on_btnHook_clicked()
{
//    HookUtil::loadHook();
//    HookUtil::setMainHwnd((HWND)winId());
//    int ret = HookUtil::setHook(HookUtil::MOUSE_LL,nullptr);

    HookUtil::setMainHwnd0((HWND)winId());
    int ret = HookUtil::setHook0(HookUtil::MOUSE_LL,nullptr);
    //HookUtil::lockMouseHook(true);
}

void HookDialog::on_btnUnHook_clicked()
{
//    HookUtil::unSetHook(HookUtil::MOUSE_LL);

    HookUtil::unSetHook0(HookUtil::MOUSE_LL);
}


bool HookDialog::nativeEvent(const QByteArray &eventType, void *message, long *result){
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        switch (msg->message) {
        case WM_USER + 100+WH_MOUSE_LL:
            //emit mousekeyboardAction();
            //qDebug() << "mouse_action"<<msg->pt.x<<msg->pt.y;
            if(msg->wParam == WM_LBUTTONDBLCLK){
                qDebug() << "WM_LBUTTONDBLCLK"<<msg->pt.x<<msg->pt.y;
            }else if(msg->wParam == WM_LBUTTONDOWN){
                qDebug() << "WM_LBUTTONDOWN"<<msg->pt.x<<msg->pt.y;
            }else if(msg->wParam == WM_LBUTTONUP){
                qDebug() << "WM_LBUTTONUP"<<msg->pt.x<<msg->pt.y;

            }
            break;
        case WM_USER + 101:
            //qDebug() << "keyboard_action";
            break;

        default:
            break;
        }
    }
#endif
    return false;
}
