#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Windows.h>
//#include <windef.h>

namespace Ui {
class Widget;
}
//class POINT;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    bool setGlobalHook(bool b);
    QString getExplorerPath(POINT point);

signals:
    void mousekeyboardAction();

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
