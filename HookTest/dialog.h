#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_btnSetHook_clicked();

    void on_btnUnSetHook_clicked();

    void on_btnSetHookSelf_clicked();

    void on_btnInjectDll_clicked();


    void on_btnSendCloseMsg_clicked();

    void on_btnLoadHook_clicked();

    void on_btnTest1_clicked();

private:
    Ui::Dialog *ui;
};


class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override;
public:
//    IShellWindows *psw = nullptr;
//    HWND hExplorer;
//    //HRESULT hr = 0;
//    bool ret = false;
//    TCHAR *g_szPath = nullptr;
};
class CUnlockHook{
public:
    ~CUnlockHook();
};

#endif // DIALOG_H
