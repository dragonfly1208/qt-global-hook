#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MyTextEdit(QWidget *parent = nullptr);

signals:

public slots:

protected:
    void dropEvent(QDropEvent*e);
};

#endif // MYTEXTEDIT_H
