#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include <QApplication>

class CApplication : public QApplication
{
public:
    CApplication(int argc, char* argv[]);

    bool notify(QObject *, QEvent *);
};

#endif // CAPPLICATION_H
