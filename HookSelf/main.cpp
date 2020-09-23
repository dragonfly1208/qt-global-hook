#include "hookdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HookDialog w;
    w.show();

    return a.exec();
}
