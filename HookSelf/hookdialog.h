#ifndef HOOKDIALOG_H
#define HOOKDIALOG_H

#include <QDialog>

namespace Ui {
class HookDialog;
}

class HookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HookDialog(QWidget *parent = nullptr);
    ~HookDialog();
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private slots:
    void on_btnHook_clicked();

    void on_btnUnHook_clicked();

private:
    Ui::HookDialog *ui;
};

#endif // HOOKDIALOG_H
