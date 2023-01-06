#include "PTcpSocket.h"

quint8 PTcpSocket::m_SEQ = 0X00;
quint8 PTcpSocket::m_MsgID = 0XFF;
quint8 PTcpSocket::m_vechicount = 0;


/**
 * @brief PTcpSocket::PTcpSocket
 * @param socketDescriptor
 * @param 使用文件描述符构造socket
 */
PTcpSocket::PTcpSocket(qintptr socketDescriptor, QObject *parent) :
    QTcpSocket(parent),socketID(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    totalTime = 0;
    successCnt = 0;
    upsuccessCnt = 0;
    downsuccessCnt = 0;
    upCnt = 0;
    downCnt = 0;
    controlCnt = 0;
    m_stressCount = 0;
    m_TcpServerTimer = new QTimer(this);
    m_HeartBeat = new Data_frame_HeartBeat;
    connect(this,&QTcpSocket::readyRead,this,&PTcpSocket::SLOT_recviceConnect);
    connect(this,&QTcpSocket::disconnected, [this](){
        emit sockDisConnect(socketID,this->peerAddress().toString(),this->peerPort());//发送断开连接的用户信息
    });
    SLOT_dealHeartBeat();
    m_TcpServerTimer->start(10000);
    connect(m_TcpServerTimer, &QTimer::timeout, this, &PTcpSocket::SLOT_dealHeartBeat);
    connect(this,&PTcpSocket::SIGNAL_Handrail_Ack,this,&PTcpSocket::SLOT_dealHandrailACK);
    connect(this,&PTcpSocket::SIGNAL_Handrail_OK,this,&PTcpSocket::SLOT_dealHandrailOK);
    connect(this,&PTcpSocket::SIGNAL_VehicleInspection,this,&PTcpSocket::SLOT_vehInfo);
    connect(this,&PTcpSocket::disconnected,this,[=](){m_TcpServerTimer->stop();});
}
/**
 * @brief PTcpSocket::UnPackFrame
 * @param FrameInfo
 * @return socket是否解包成功
 */
void PTcpSocket::UnPackFrame(QByteArray FrameInfo)
{
    int len = FrameInfo.size();
    switch (len) {
    case 0X05:
    {
        m_heartBeat = reinterpret_cast<HeartBeat*>(FrameInfo.data());
        break;
    }
#ifndef Isclient
    case 0x0E:
    {
        VehicleInspection *pCarInfo = reinterpret_cast<VehicleInspection*>(FrameInfo.data());
        emit SIGNAL_VehicleInspection(pCarInfo);
        break;
    }
    case 0X04:
    {
        m_handrailOK = reinterpret_cast<Handrail_OK*>(FrameInfo.data());
        emit SIGNAL_Handrail_OK(m_handrailOK);
        break;
    }
    case 0X017:
    {
        m_handrail_Ack = reinterpret_cast<Handrail_Ack*>(FrameInfo.data());
        emit SIGNAL_Handrail_Ack(m_handrail_Ack);
        break;
    }

#endif
    case 0X09:
    {
        m_handrail_Request = reinterpret_cast<Handrail_Request*>(FrameInfo.data());
        emit SIGNAL_Handrail_Request(m_handrail_Request);
        break;
    }
    default:
        break;
    }
}

quint16 PTcpSocket::calc_crc16(quint16 init_crc, quint8 *crc_data, int len)
{
    int crc = init_crc, cnt, k;
    for (cnt = 0; cnt < len; cnt++) {
        //loop data length
        crc ^= crc_data[cnt];
        for (k = 0; k < 8; k++) {
            //every bit data
            if (crc & 0x1) {
                crc = (crc >> 1) ^ 0X8408;
            }
            else {
                crc = (crc >> 1);
            }
        }
    }
    crc = (~crc) & 0xFFFF;
    return static_cast<quint16>(crc);
}
/**
 * @brief PTcpSocket::SLOT_recviceConnect
 * @note 接收到的数据处理
*/
void PTcpSocket::SLOT_recviceConnect()
{
    m_LastTime = QDateTime::currentDateTime();
    QByteArray pRecvData = readAll();
    qDebug()<<pRecvData.toHex().toUpper();
    QByteArray pSTX = pRecvData.mid(0,2);
    if(pSTX == "\xFF\xFF")
    {
        QByteArray a = pRecvData.mid(4,1);
        quint8* pFrameData = reinterpret_cast<quint8*>(pRecvData.data());
        QByteArray bCRC = pRecvData.right(2);
        quint16 *crc = reinterpret_cast<quint16*>(bCRC.data());
        if(calc_crc16(0XFFFF,pFrameData + 2,pRecvData.size() - 4) == *crc)
        {
            QByteArray bLen = pRecvData.mid(8,1);
            quint8 *CmdType = reinterpret_cast<quint8*>(bLen.data());
            qDebug()<<"cmdType = "<<hex<<*CmdType;
            switch (*CmdType) {
            //车检解析
            case 0XB1:
            {
                QByteArray carInfo = pRecvData.mid(8,14);
                UnPackFrame(carInfo);
                break;
            }
#ifndef IsServer
                //请求包解析
            case 0XA1:
            {
                QByteArray request = pRecvData.mid(8,9);
                UnPackFrame(request);
                break;
            }
#endif
#ifndef Isclient
                //应答包解析
            case 0XA1:
            {
                QByteArray ACK = pRecvData.mid(8,23);
                UnPackFrame(ACK);
                break;
            }
#endif
                //抬落杆到位解析
            case 0XA2:
            {
                QByteArray ok = pRecvData.mid(8,4);
                UnPackFrame(ok);
                break;
            }
                //心跳包解析
            case 0XED:
            {
                QByteArray heartBeat = pRecvData.mid(8,5);
                UnPackFrame(heartBeat);
                break;
            }
            default:
                break;
            }
        }else
        {
            qDebug()<<"CRC验证错误";
        }

    }
    else
    {
        m_Msg.clear();
        qDebug()<<"丢弃无效数据";
    }
}



void PTcpSocket::SLOT_dealHeartBeat()
{
    m_HeartBeat->SEQ = PTcpSocket::m_SEQ + 0X01;
    m_HeartBeat->LEN = 0X05;
    HeartBeat heartBeat;
    heartBeat.UnixTm = static_cast<quint32>(QDateTime::currentDateTime().toSecsSinceEpoch());
    m_HeartBeat->DATA = heartBeat;

    quint8 *crcGet = reinterpret_cast<quint8*>(m_HeartBeat);
    m_HeartBeat->CRC = calc_crc16(0XFFFF, crcGet + 2, 11);
    write(reinterpret_cast<char*>(m_HeartBeat), sizeof(*m_HeartBeat));
}
/**
 * @brief PTcpSocket::SLOT_vehInfo
 * @param vehicleInfo
 * @note 车检信息处理
 */
void PTcpSocket::SLOT_vehInfo(VehicleInspection *vehicleInfo)
{
    m_vechicount++;
    if(vehicleInfo->Status == 1)
    {
        emit signal_haveVehInfo(static_cast<int>(socketID),"有车",m_vechicount);
    }else
    {
        emit signal_haveVehInfo(static_cast<int>(socketID),"无车",m_vechicount);
    }
}

void PTcpSocket::SLOT_dealHandrailACK(Handrail_Ack *ack)
{
    if(ack->ErrCode == 0X00)  //控制成功
    {
        controlCnt++;
        m_signalTime = ack->UnixTm2 - ack->UnixTm1;
        totalTime += m_signalTime;
    }
}
/**
 * @brief PTcpSocket::SLOT_dealHandrailOK
 * @param ok
 * @note  栏杆机回应应答处理
 */
void PTcpSocket::SLOT_dealHandrailOK(Handrail_OK *ok)
{
    //ui.raiseWgt->setRowCount(controlCnt);
    emit signal_TowCount(static_cast<int>(socketID),controlCnt);
    QString type;
    if(ok->BarStaus == 0X00)
    {
        QStringList stringList;
        stringList.append(QString("第%1次落杆").arg(downCnt));
        stringList.append("成功");
        stringList.append(QString("耗时%1毫秒").arg(m_signalTime));
        emit SIGNAL_haveDownTableItem(static_cast<int>(socketID),stringList.at(0),stringList.at(1),stringList.at(2));
        successCnt++;
        downsuccessCnt++;

    }else if(ok->BarStaus == 0X01)
    {
        QStringList stringList;
        stringList.append(QString("第%1次抬杆").arg(upCnt));
        stringList.append("成功");
        stringList.append(QString("耗时%1毫秒").arg(m_signalTime));
        emit SIGNAL_haveUpTableItem(static_cast<int>(socketID),stringList.at(0),stringList.at(1),stringList.at(2));
        successCnt++;
        upsuccessCnt++;
    }
    //控制计数
    qreal rate;
    qreal up = static_cast<qreal>(upCnt);
    qreal down =static_cast<qreal>(downCnt);
    qreal temp = static_cast<qreal>(successCnt);
    qreal tempTime =static_cast<qreal>(totalTime);

    //控制比率
    rate = (temp / (up + down )) * 100;
    emit signal_haveTotalTime(static_cast<int>(socketID),QString("%1毫秒").arg(totalTime));
    qreal time = tempTime / (up + down);
    emit signal_haveavgTime(static_cast<int>(socketID),QString("%1毫秒").arg(time));
    emit signal_havesuccessRate(static_cast<int>(socketID),QString("%1%").arg(QString::number(rate,'g',3)));

    //static QMutex mutex;
    //mutex.lock();
    QString str = QString("客户端%1|总耗时%2毫秒|平均耗时%3毫秒| 成功率%4%").arg(socketID).arg(totalTime).arg(QString::number(time,'g',3)).arg(QString::number(rate,'g',3));
    QFile file(QString("%1Log.txt").arg(socketID));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream.setCodec("utf-8");
    text_stream << str << "\r\n";
    file.flush();
    file.close();
    //mutex.unlock();
}

void PTcpSocket::slot_upTest(int handle)
{
    qDebug()<<"run this"<<__FUNCTION__<<__LINE__<<"handle = "<<handle<<"SOckId="<<socketID;
    if(handle == socketID)
    {
        upCnt++;
        testStressToClient(0X01);
        write(reinterpret_cast<char*>(m_HandrailRequest), sizeof(*m_HandrailRequest));
    }
}

void PTcpSocket::slot_downTest(int handle)
{
    if(handle == socketID)
    {
        downCnt++;
        testStressToClient(0X00);
        write(reinterpret_cast<char*>(m_HandrailRequest), sizeof(*m_HandrailRequest));
    }
}
/**
 * @brief PTcpSocket::slot_stressTest
 * @note 压力测试
 */
void PTcpSocket::slot_stressTest(int handle)
{
    if(handle == socketID)
    {
        stressFlag = true;
        m_stressTest = new QTimer(this);
        m_stressTest->setInterval(1800);
        m_stressTest->start();
        m_stressCount = 0;
        connect(m_stressTest,&QTimer::timeout,this,[=](){
            m_stressCount++;
            if(m_stressCount >= 20 && m_stressTest != nullptr)
            {
                stressFlag = false;
                m_stressTest->stop();
                m_stressTest->deleteLater();
                m_stressTest = nullptr;
            }

            if(m_stressCount % 2 == 0)
            {
                upCnt++;
                testStressToClient(0X01);
                write(reinterpret_cast<char*>(m_HandrailRequest), sizeof(*m_HandrailRequest));

            }else{
                downCnt++;
                testStressToClient(0X00);
                write(reinterpret_cast<char*>(m_HandrailRequest), sizeof(*m_HandrailRequest));
            }
        });
    }
}

void PTcpSocket::stopHeart()
{
    m_TcpServerTimer->stop();
    qDebug()<<socketID<<"心跳停止";
}

void PTcpSocket::startHeart()
{
    m_TcpServerTimer->setInterval(10000);
    m_TcpServerTimer->start();
}
/**
 * @brief PTcpSocket::testStressToClient
 * @param warmStatus 抬/落杆
 * @note 封包
 */
void PTcpSocket::testStressToClient(quint8 warmStatus)
{
    m_HandrailRequest = new Data_frame_Handrail_Request;
    m_HandrailRequest->SEQ = PTcpSocket::m_SEQ + 0X01;
    m_HandrailRequest->LEN = sizeof(Handrail_Request);
    Handrail_Request handrailRequest;

    handrailRequest.DeviceNo = 0X01;
    handrailRequest.MsgID = PTcpSocket::m_MsgID + 0X01;

    handrailRequest.WarmStaus = warmStatus;
    m_HandrailRequest->DATA = handrailRequest;
    m_HandrailRequest->CRC = calc_crc16(0XFFFF, &m_HandrailRequest->VER, sizeof(Data_frame_Handrail_Request) - 4);
}
