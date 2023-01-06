#ifndef FRAME_H
#define FRAME_H
#include <QObject>
#include <QTcpSocket>
#include "DataStruct.h"
#include <QDateTime>
#include <QTimer>
#include <QHostAddress>
#include <QString>
#include <QFile>
#include <QMutex>
class PTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit PTcpSocket(qintptr socketDescriptor,QObject *parent = nullptr);
    /**
     * @brief PackFrame
     * @return
     * @note 解包
     */
    void UnPackFrame(QByteArray FrameInfo);
    quint16 calc_crc16(quint16 init_crc, quint8* crc_data, int len);

    //界面交互 用于测试客户端 压测 单测等
    void testStressToClient(quint8 warmStatus);
signals:
    void SIGNAL_OverTime();
    void SIGNAL_Handrail_Request(Handrail_Request*);
    void SIGNAL_VehicleInspection(VehicleInspection*);
    void SIGNAL_Handrail_Ack(Handrail_Ack*);
    void SIGNAL_Handrail_OK(Handrail_OK*);
public slots:
    void SLOT_recviceConnect();
    //界面交互测试

    void slot_upTest(int handle);
    void slot_downTest(int handle);
    void slot_stressTest(int handle);

    //心跳暂停恢复
    void stopHeart();
    void startHeart();
private slots :  //主线程复制
    void SLOT_dealHeartBeat();

    void SLOT_vehInfo(VehicleInspection *vehicleInfo);
    void SLOT_dealHandrailACK(Handrail_Ack *ack);
    void SLOT_dealHandrailOK(Handrail_OK *ok);
signals:
    //  void readData(const int,const QString &,const quint16,const QByteArray &);//发送获得用户发过来的数据
    void sockDisConnect(qintptr ,const QString &,const quint16 );//断开连接的用户信息

    //与界面交互
    void signal_TowCount(int,int);
    void signal_haveVehInfo(int,QString,int);
    void SIGNAL_haveUpTableItem(int,QString,QString,QString);
    void SIGNAL_haveDownTableItem(int,QString,QString,QString);
    void signal_haveTotalTime(int handle,QString Info);
    void signal_haveavgTime(int handle,QString Info);
    void signal_havesuccessRate(int handle,QString Info);
public slots:
    //   void thisReadData();//处理接收到的数据
    //  void sentData(const QByteArray & ,const int);//发送信号的槽
private:
    qintptr socketID;//保存id，== this->socketDescriptor()；但是this->socketDescriptor()客户端断开会被释放，
    //断开信号用this->socketDescriptor()得不到正确值
private:
    VehicleInspection           *m_vehicleInspection;
    Handrail_Request            *m_handrail_Request;
    Handrail_Ack                *m_handrail_Ack;
    Handrail_OK                 *m_handrailOK;
    HeartBeat                   *m_heartBeat;
    QByteArray                  m_Msg;

    QDateTime                   m_LastTime;


    static quint8                    m_SEQ;
    static quint8                    m_MsgID;
    static quint8                    m_vechicount;
    Data_frame_Handrail_Request		*m_HandrailRequest;
    Data_frame_VehicleInspection	*m_Car_Report;
    Data_frame_HeartBeat            *m_HeartBeat;
    QTimer                          *m_TcpServerTimer;

    quint64                         m_signalTime;
    quint64                         totalTime;
    quint8                          controlCnt;
    quint8                          upsuccessCnt;
    quint8                          downsuccessCnt;
    quint8                          successCnt;
    quint8                          upCnt;
    quint8                          downCnt;
    QTimer                          *m_stressTest;
    quint8                          m_stressCount;
    bool                            stressFlag;
};

#endif // FRAME_H
