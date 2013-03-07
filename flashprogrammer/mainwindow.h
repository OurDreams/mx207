#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "cm3updater.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    cm3updater *cm3updt;
    QString cfgfileName;        //配置文件
    QList<QextPortInfo> ports;  //串口列表
    void setAllCtrlEnabled(bool status);

private slots:
    void on_choiceBtn_clicked();
    void on_writeBtn_clicked();
    void on_refreshBtn_clicked();
    void on_updtStatus_changed(int, int argc);
    void on_updtThread_return();
};

#endif // MAINWINDOW_H
