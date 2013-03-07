#ifndef CM3UPDATER_H
#define CM3UPDATER_H

#include <QThread>
#include <Qfile>
#include "qextserialport.h"

//延时，TIME_OUT是串口读写的延时(单位ms)
#define TIME_OUT 10 * 1

#define MAX_REPEAT_TIME     5
#define TRANSFER_START      0   //起始地址必须为0，由各BootLoader自行决定写入位置
#define MAX_DATASIZE        252

#define     UPDT_NONE       0
#define     UPDT_PING       1
#define     UPDT_PING_ERR   2   //超时
#define     UPDT_ERASE      3
#define     UPDT_ERASE_ERR  4
#define     UPDT_DATA       5
#define     UPDT_DATA_ERR   6
#define     UPDT_REST       7
#define     UPDT_REST_ERR   8
#define     UPDT_OK         9   //升级成功
#define     UPDT_COM_ERR    10  //串口打开失败
#define     UPDT_FILE_ERR   11  //目标文件打开失败

class cm3updater:public QThread
{
    Q_OBJECT

public:
    cm3updater(QString fportName, QString speed, QString fbinfileName);

private:
    QString portName;
    QString speed;
    QString binfileName;
    unsigned mytimeout;
    int status;    //升级状态
    QextSerialPort *com;  //串口对象
    QFile *srcFile;

    int AckPacket(void);
    int NakPacket(void);
    int GetPacket(unsigned char *pucData, unsigned char *pucSize);
    int SendPacket(unsigned char *pucData, unsigned char ucSize,
                   unsigned long bAck);
    unsigned char CheckSum(unsigned char *pucData, unsigned char ucSize);
    int SendCommand(unsigned char *pucCommand, unsigned char ucSize);
    bool pingTarget();
    bool eraseCmd();
    bool dataCmd();
    bool resetCmd();

protected:
    void run();

signals:
    void sigStatusChanged(int status, int argc);
};

#endif // CM3UPDATER_H
