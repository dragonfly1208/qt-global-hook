#include "mytextedit.h"

MyTextEdit::MyTextEdit(QWidget *parent) : QTextEdit(parent)
{

}


void MyTextEdit::dropEvent(QDropEvent*e){

    QTextEdit::dropEvent(e);
}
