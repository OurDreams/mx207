#include <QApplication>
#include "mainwindow.h"
#include "qtextcodec.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    MainWindow w;
    w.show();
    
    return a.exec();
}
