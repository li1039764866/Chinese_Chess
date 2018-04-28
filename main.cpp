#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.Initialize_Array();//窗体显示的时候各棋子位置已经到位
    w.show();
    return a.exec();
}
