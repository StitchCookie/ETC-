#ifndef ETCTcpServer_H
#define ETCTcpServer_H

#include <QObject>
#include <QTcpServer>
#include <QScopedPointer>
#include <QTcpSocket>
#include <QMap>
#include "qobject.h"
#include <TCP_PUBLIC/PTcpSocket.h>
#include <QThread>
class ETCTcpServerPrivate;
class Q_DECL_EXPORT ETCTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit ETCTcpServer(QObject *parent = nullptr);
    QMap<int,PTcpSocket *>*  getSocketSInfo();
    ~ETCTcpServer();
    //继承QTCPSERVER以实现多线程TCPscoket的服务器。
signals:
    void connectClient(const int , const QString & ,const quint16 );//发送新用户连接信息
    void sockDisConnected(const int ,const QString &,const quint16 );//断开连接的用户信息
    void SIGNAL_upTest(int handle);
    void SIGNAL_downTest(int handle);
    void SIGNAL_stressTest(int handle);
    //与界面交互
    void signal_TowCount(int,int);
    void signal_haveVehInfo(int,QString,int);
    void signal_haveUpTableItem(int,QString,QString,QString);
    void signal_haveDownTableItem(int,QString,QString,QString);
    void signal_haveTotalTime(int handle,QString Info);
    void signal_haveavgTime(int handle,QString Info);
    void signal_havesuccessRate(int handle,QString Info);
private slots:
    void sockDisConnectSlot(qintptr handle, QString ip, quint16 prot);//断开连接的用户信息

protected:
    void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程
    QScopedPointer<ETCTcpServerPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(ETCTcpServer)
   //Q_DISABLE_COPY(ETCTcpServer)
};

#endif // ETCTcpServer_H

