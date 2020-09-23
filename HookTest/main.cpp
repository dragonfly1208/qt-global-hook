#include "dialog.h"
#include <QApplication>
#include "capplication.h"

int main(int argc, char *argv[])
{
    CApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
