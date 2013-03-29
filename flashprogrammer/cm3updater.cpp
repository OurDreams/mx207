#include "cm3updater.h"
#include <QDebug>


#define COMMAND_PING                0x20
#define COMMAND_DOWNLOAD            0x21
#define COMMAND_RUN                 0x22
#define COMMAND_GET_STATUS          0x23
#define COMMAND_SEND_DATA           0x24
#define COMMAND_RESET               0x25

#define COMMAND_RET_SUCCESS         0x40
#define COMMAND_RET_UNKNOWN_CMD     0x41
#define COMMAND_RET_INVALID_CMD     0x42
#define COMMAND_RET_INVALID_ADDR    0x43
#define COMMAND_RET_FLASH_FAIL      0x44
#define COMMAND_ACK                 0xcc
#define COMMAND_NAK                 0x33

cm3updater::cm3updater(QString fportName, QString speed, QString fbinfileName)
{
    status = UPDT_NONE;
    portName = fportName;
    binfileName = fbinfileName;
    mytimeout = 200;  //2s
}

//****************************************************************************
//
//! AckPacket() sends an Acknowledge a packet.
//!
//! This function acknowledges a packet has been received from the device.
//!
//! \return The function returns zero to indicated success while any non-zero
//! value indicates a failure.
//
//****************************************************************************
int cm3updater::AckPacket(void)
{
    char ucAck;

    ucAck = COMMAND_ACK;
    return !(com->write(&ucAck, 1) == 1);
}

//****************************************************************************
//
//! NakPacket() sends a No Acknowledge packet.
//!
//! This function sends a no acknowledge for a packet that has been
//! received unsuccessfully from the device.
//!
//! \return The function returns zero to indicated success while any non-zero
//! value indicates a failure.
//
//****************************************************************************
int cm3updater::NakPacket(void)
{
    char ucNak;

    ucNak = COMMAND_NAK;
    return !(com->write(&ucNak, 1) == 1);
}

//*****************************************************************************
//
//! GetPacket() receives a data packet.
//!
//! \param pucData is the location to store the data received from the device.
//! \param pucSize is the number of bytes returned in the pucData buffer that
//! was provided.
//!
//! This function receives a packet of data from UART port.
//!
//! \returns The function returns zero to indicated success while any non-zero
//! value indicates a failure.
//
//*****************************************************************************
int cm3updater::GetPacket(unsigned char *pucData, unsigned char *pucSize)
{
    unsigned char ucCheckSum;
    unsigned char ucSize;
    int time_out = mytimeout; //2s

    //
    // Get the size waht and the checksum.
    //
    do
    {
        if(com->read((char *)&ucSize, 1) != 1)
        {
            this->msleep(10);
            if (time_out-- > 0)
                continue;
            return(-1);
        }
    }
    while(ucSize == 0);

    time_out = mytimeout;
    while (com->read((char *)&ucCheckSum, 1) != 1)
    {
        this->msleep(10);
        if (time_out-- > 0)
            continue;
        return(-1);
    }
    *pucSize = ucSize - 2;

    time_out = mytimeout;
    while (com->bytesAvailable() < (qint64)*pucSize) {
        this->msleep(10);
        if (time_out-- > 0)
            continue;
        return(-1);
    }

    qDebug("pack sizes:%d", *pucSize);
    if(com->read((char *)pucData, *pucSize) != *pucSize)
    {
        *pucSize = 0;
        return(-1);
    }

    //
    // Calculate the checksum from the data.
    //
    if(CheckSum(pucData, *pucSize) != ucCheckSum)
    {
        *pucSize = 0;
        return(NakPacket());
    }

    return(AckPacket());
}

//*****************************************************************************
//
//! CheckSum() Calculates an 8 bit checksum
//!
//! \param pucData is a pointer to an array of 8 bit data of size ucSize.
//! \param ucSize is the size of the array that will run through the checksum
//!     algorithm.
//!
//! This function simply calculates an 8 bit checksum on the data passed in.
//!
//! \return The function returns the calculated checksum.
//
//*****************************************************************************
unsigned char cm3updater::CheckSum(unsigned char *pucData, unsigned char ucSize)
{
    int i;
    unsigned char ucCheckSum;

    ucCheckSum = 0;

    for(i = 0; i < ucSize; ++i)
    {
        ucCheckSum += pucData[i];
    }
    return(ucCheckSum);
}

//*****************************************************************************
//
//! SendPacket() sends a data packet.
//!
//! \param pucData is the location of the data to be sent to the device.
//! \param ucSize is the number of bytes to send from puData.
//! \param bAck is a boolean that is true if an ACK/NAK packet should be
//! received in response to this packet.
//!
//! This function sends a packet of data to the device.
//!
//! \returns The function returns zero to indicated success while any non-zero
//!     value indicates a failure.
//
//*****************************************************************************
int cm3updater::SendPacket(unsigned char *pucData, unsigned char ucSize, unsigned long bAck)
{
    unsigned char ucCheckSum;
    unsigned char ucAck;

    ucCheckSum = CheckSum(pucData, ucSize);

    //
    // Make sure that we add the bytes for the size and checksum to the total.
    //
    ucSize += 2;

    //
    // Send the Size in bytes.
    //
    if(com->write((char *)&ucSize, 1) != 1)
    {
        qDebug("%d err", __LINE__);
        return(-1);
    }

    //
    // Send the CheckSum
    //
    if(com->write((char *)&ucCheckSum, 1) != 1)
    {
        qDebug("%d err", __LINE__);
        return(-1);
    }

    //
    // Now send the remaining bytes out.
    //
    ucSize -= 2;

    //
    // Send the Data
    //
    if(com->write((char *)pucData, ucSize) != ucSize)
    {
        qDebug("%d err", __LINE__);
        return(-1);
    }

    //
    // Return immediately if no ACK/NAK is expected.
    //
    if(!bAck)
    {
        return(0);
    }

    ucAck = 0;
    int time_out = mytimeout * 4;
    //
    // Wait for the acknoledge from the device.
    //
    do
    {
        if(com->read((char *)&ucAck, 1) != 1)
        {
            this->msleep(10);
            if (time_out-- > 0)
                continue;
            qDebug("%d err", __LINE__);
            return(-1);
        }
    }
    while(ucAck == 0);

    if(ucAck != COMMAND_ACK)
    {
        qDebug("%d err", __LINE__);
        return(-1);
    }
    return(0);
}

int cm3updater::SendCommand(unsigned char *pucCommand, unsigned char ucSize)
{
    unsigned char ucStatus;

    //
    // Send the command itself.
    //
    if(SendPacket(pucCommand, ucSize, 1) < 0)
    {
        qDebug("command itself err");
        return(-1);
    }

    //
    // Send the get status command to tell the device to return status to
    // the host.
    //
    ucStatus = COMMAND_GET_STATUS;
    if(SendPacket(&ucStatus, 1, 1) < 0)
    {
        qDebug("Failed to Get Status\n");
        return(-1);
    }

    //
    // Read back the status provided from the device.
    //
    ucSize = sizeof(ucStatus);
    if(GetPacket(&ucStatus, &ucSize) < 0)
    {
        qDebug("Failed to Get Packet\n");
        return(-1);
    }
    if(ucStatus != COMMAND_RET_SUCCESS)
    {
        qDebug("Failed to get download command Return Code: %04x\n",
            ucStatus);
        return(-1);
    }
    return(0);
}


bool cm3updater::pingTarget()
{
    int i;
    unsigned char hello[3] = {3, 0x20, COMMAND_PING};

    status = UPDT_PING;
    mytimeout = 50;
    for (i = 0; i < MAX_REPEAT_TIME; i++) {
        qDebug("pingTarget:%d", i + 1);
        emit sigStatusChanged(status, i);
        if (SendPacket(hello, 3, 1) == 0) {
            break;
        }
        this->msleep(500);
    }
    mytimeout = 200;

    if (i >= MAX_REPEAT_TIME) {
        return false;
    }

    return true;
}

bool cm3updater::eraseCmd()
{
    unsigned char g_ucBuffer[9];
    unsigned long ulTransferLength;

    status = UPDT_ERASE;
    emit sigStatusChanged(status, 0);
    ulTransferLength = srcFile->size();
    qDebug("file size:%ld", ulTransferLength);
    qDebug("char len:%d", sizeof(char));
    g_ucBuffer[0] = COMMAND_DOWNLOAD;
    g_ucBuffer[1] = (unsigned char)(TRANSFER_START >> 24);
    g_ucBuffer[2] = (unsigned char)(TRANSFER_START >> 16);
    g_ucBuffer[3] = (unsigned char)(TRANSFER_START >> 8);
    g_ucBuffer[4] = (unsigned char)TRANSFER_START;
    g_ucBuffer[5] = (unsigned char)(ulTransferLength>>24);
    g_ucBuffer[6] = (unsigned char)(ulTransferLength>>16);
    g_ucBuffer[7] = (unsigned char)(ulTransferLength>>8);
    g_ucBuffer[8] = (unsigned char)ulTransferLength;
    if (SendCommand(g_ucBuffer, 9) < 0)
    {
        qDebug("Failed to Send Download Command");
        return false;
    }
    return true;
}

bool cm3updater::dataCmd()
{
    unsigned char g_ucBuffer[256];
    unsigned long ulOffset = 0;
    unsigned long filesize = srcFile->size();
    unsigned long ulTransferLength = filesize;

    status = UPDT_DATA;
    do
        {
            unsigned char ucBytesSent;

            g_ucBuffer[0] = COMMAND_SEND_DATA;

            qDebug("%08ld", ulTransferLength);

            //
            // Send out 8 bytes at a time to throttle download rate and avoid
            // overruning the device since it is programming flash on the fly.
            //
            if(ulTransferLength >= MAX_DATASIZE)
            {
                srcFile->read((char *)&g_ucBuffer[1], MAX_DATASIZE);

                ulOffset += MAX_DATASIZE;
                ulTransferLength -= MAX_DATASIZE;
                ucBytesSent = MAX_DATASIZE + 1;
            }
            else
            {
                srcFile->read((char *)&g_ucBuffer[1], ulTransferLength);
                ulOffset += ulTransferLength;
                ucBytesSent = ulTransferLength + 1;
                ulTransferLength = 0;
            }
            //
            // Send the Send Data command to the device.
            //
            if(SendCommand(g_ucBuffer, ucBytesSent) < 0)
            {
                qDebug("Failed to Send Packet data\n");
                break;
            }

            emit sigStatusChanged(status, ulOffset * 100 / filesize);
        } while (ulTransferLength);

    if (ulTransferLength)   //若中途出错则返回错误
        return false;
    return true;
}

bool cm3updater::resetCmd()
{
    status = UPDT_REST;
    emit sigStatusChanged(status, 0);
    //
    // Send the reset command but just send the packet, there will likely
    // be no boot loader to answer after this command completes.
    //
    unsigned char g_ucBuffer = COMMAND_RESET;
    SendPacket(&g_ucBuffer, 1, 0);
    return true;
}

void cm3updater::run()
{
    com = new QextSerialPort(portName, QextSerialPort::Polling);

    if(false == com->open(QIODevice::ReadWrite)) {
        qDebug("com open err!");
        status = UPDT_COM_ERR;
        goto __free0;
    }
    srcFile = new QFile(binfileName);

    if (false == srcFile->open(QFile::ReadOnly)) {
        qDebug("bin file open is not exists!");
        status = UPDT_FILE_ERR;
        goto __free1;
    }

    // 初始化串口
    if (speed == "115200")
        com->setBaudRate(BAUD115200);   //设置波特率
    if (speed == "38400")
        com->setBaudRate(BAUD38400);   //设置波特率
    if (speed == "9600")
        com->setBaudRate(BAUD9600);   //设置波特率

    com->setBaudRate(BAUD115200);   //设置波特率
    com->setDataBits(DATA_8);       //设置数据位
    com->setParity(PAR_NONE);       //设置校验
    com->setStopBits(STOP_1);       //设置停止位
    com->setFlowControl(FLOW_OFF);  //设置数据流控制
    com->setTimeout(TIME_OUT);      //设置延时
    com->flush();
    qDebug("bytesAvailable:%d", (int)com->bytesAvailable());
    //1. 测试连接
    if (false == pingTarget()){
        qDebug("send hello timeout!");
        status = UPDT_PING_ERR;
        goto __free1;
    }

    //2. 擦除
    if (false == eraseCmd()) {
        status = UPDT_ERASE_ERR;
        goto __free1;
    }

    //3. 数据
    if (false == dataCmd()) {
        status = UPDT_DATA_ERR;
        goto __free1;
    }

    //4. 复位
    if (false == resetCmd()) {
        status = UPDT_REST_ERR;
        goto __free1;
    }
    status = UPDT_OK;

__free1:
    srcFile->close();
    delete srcFile;
__free0:
    com->close();
    delete com;
    emit sigStatusChanged(status, 0);

    return ;
}
