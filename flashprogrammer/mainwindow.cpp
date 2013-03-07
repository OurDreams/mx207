#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include "qfiledialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->Button_choice, SIGNAL(clicked()), this, SLOT(on_choiceBtn_clicked()));
    connect(ui->Button_write, SIGNAL(clicked()), this, SLOT(on_writeBtn_clicked()));
    connect(ui->Button_refresh, SIGNAL(clicked()), this, SLOT(on_refreshBtn_clicked()));
    //connect(cm3updt, SIGNAL(sigStatusChanged(updtStatus,int)),
    //        this, SLOT(on_updtStatus_changed(updtStatus,int)));

    //ui->label_statuspic->hide();
    //ui->label_status->hide();

    ports = QextSerialEnumerator::getPorts();
    for (int i = ports.size() - 1; i >= 0; i--) {
        ui->comboBox_comno->addItem(ports.at(i).portName.toLocal8Bit().constData(), i);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAllCtrlEnabled(bool status)
{
    ui->Button_close->setDisabled(status);
    ui->Button_choice->setDisabled(status);
    ui->Button_refresh->setDisabled(status);
    ui->Button_write->setDisabled(status);
    ui->label_com->setDisabled(status);
    ui->label_baudrate->setDisabled(status);
    ui->comboBox_braudrate->setDisabled(status);
    ui->comboBox_comno->setDisabled(status);
}

void MainWindow::on_choiceBtn_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("打开执行文件"), ".", tr("Image Files(*.bin)"));
    if (path.size() != 0)
        this->ui->lineEdit_srcfile->setText(path);
}

void MainWindow::on_writeBtn_clicked()
{
    setAllCtrlEnabled(true);

    ui->progressBar->setValue(0);
    QString portName = ui->comboBox_comno->currentText();   //获取串口名
    QString binFileName = ui->lineEdit_srcfile->text();     //目标文件名
    QString speed = ui->comboBox_braudrate->currentText();  //波特率

    cm3updt = new cm3updater(portName, speed, binFileName);
    connect(cm3updt, SIGNAL(sigStatusChanged(int, int)),
            this, SLOT(on_updtStatus_changed(int, int)));
    connect(cm3updt, SIGNAL(finished()), this, SLOT(on_updtThread_return()));
    cm3updt->start();
}

void MainWindow::on_updtStatus_changed(int status, int argc)
{
    switch (status) {
    case UPDT_NONE:
        ui->label_status->clear();
        ui->label_statuspic->clear();
        break;

    case UPDT_PING:
        ui->label_status->setText(tr("检测链路(%1)...").arg(argc)); //TODO: argc次数
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/set_action_obj.gif"));
        break;

    case UPDT_ERASE:
        ui->label_status->setText(tr("正在擦除..."));
        break;

    case UPDT_ERASE_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>擦除失败!</font></h2>"));
        QApplication::beep();
        break;

    case UPDT_DATA:
        ui->label_status->setText(tr("正在写入(%1%)...").arg(argc));   //argc百分比
        qDebug("per:%d", argc);
        ui->progressBar->setValue(argc);
        break;

    case UPDT_DATA_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>写失败!</font></h2>"));
        QApplication::beep();
        break;


    case UPDT_REST:
        ui->label_status->setText(tr("复位..."));
        break;

    case UPDT_REST_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>复位失败!</font></h2>"));
        QApplication::beep();
        break;

    case UPDT_OK:
        ui->label_status->setText(tr("写入成功."));
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/ok.gif"));
        QApplication::beep();
        break;

    case UPDT_COM_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>写入失败(串口被其它程序占用).</font></h2>"));
        QApplication::beep();
        break;

    case UPDT_FILE_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>目标文件打开失败!</font></h2>"));
        QMessageBox::critical(this, tr("注意"),  tr("目标文件不存在"), QMessageBox::Ok);
        break;

    case UPDT_PING_ERR:
        ui->label_statuspic->setPixmap(QPixmap(":/new/perfix1/img/fatalerror_obj.gif"));
        ui->label_status->setText(tr("<font color=red>写入失败(等待设备响应超时).</font></h2>"));
        QApplication::beep();
        break;
    }
}

void MainWindow::on_refreshBtn_clicked()
{
    ports = QextSerialEnumerator::getPorts();
    ui->comboBox_comno->clear();
    for (int i = ports.size() - 1; i >= 0; i--) {
        ui->comboBox_comno->addItem(ports.at(i).portName.toLocal8Bit().constData(), i);
    }
}

void MainWindow::on_updtThread_return()
{
    delete cm3updt;
    setAllCtrlEnabled(false);
}
