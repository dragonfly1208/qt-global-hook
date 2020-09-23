#include "capplication.h"
#include <QDebug>

CApplication::CApplication(int argc, char* argv[])
    :QApplication(argc, argv)
{

}

bool CApplication::notify(QObject *obj, QEvent *ev)
{
    //qDebug() << obj->objectName()<<ev->type();
    // 根据窗口名字判断是否是 CWidget 窗口类的消息
//    if (obj->objectName() == "widgets")
//    {
//        // 判断消息类型是不是鼠标点击消息
//        if (ev->type() == QEvent::MouseButtonPress)
//        {
//            qDebug() << "在窗口中点击了鼠标！";
//        }
//    }
    // 将消息交还给父窗口处理，避免消息丢失
    return QApplication::notify(obj, ev);
}
