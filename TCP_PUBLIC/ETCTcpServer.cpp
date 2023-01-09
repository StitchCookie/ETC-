#include "ETCTcpServer.h"
class ETCTcpServerPrivate {
    Q_DECLARE_PUBLIC(ETCTcpServer)
public:
    ETCTcpServerPrivate(ETCTcpServer *parent) : q_ptr(parent){
        //Q_Q(ETCTcpServer);
        tcpClient = new QMap<int,PTcpSocket*>;
    }

private:
    ETCTcpServer *q_ptr;
    QMap<int,PTcpSocket *>   *tcpClient;
};

/**
 * @brief ETCTcpServer::incomingConnection
 * @param socketDescriptor
 * @note 重新incoming虚函数 实现多线程服务器处理客户端
 */

void ETCTcpServer::incomingConnection(qintptr socketDescriptor)
{
    Q_D(ETCTcpServer);
    PTcpSocket *tcpTemp = new PTcpSocket(socketDescriptor);
    QThread *thread = new QThread(tcpTemp);
    d->tcpClient->insert(socketDescriptor,tcpTemp);//插入到连接信息中
    emit connectClient(tcpTemp->socketDescriptor(),tcpTemp->peerAddress().toString(),tcpTemp->peerPort());
    connect(tcpTemp,&PTcpSocket::disconnected,thread,[=](){thread->quit();});//断开连接时线程退出
    connect(tcpTemp,&PTcpSocket::signal_TowCount,this,&ETCTcpServer::signal_TowCount);
    connect(tcpTemp,&PTcpSocket::signal_haveVehInfo,this,&ETCTcpServer::signal_haveVehInfo);
    connect(tcpTemp,&PTcpSocket::SIGNAL_haveUpTableItem,this,&ETCTcpServer::signal_haveUpTableItem,Qt::UniqueConnection);
    connect(tcpTemp,&PTcpSocket::SIGNAL_haveDownTableItem,this,&ETCTcpServer::signal_haveDownTableItem,Qt::UniqueConnection);
    connect(tcpTemp,&PTcpSocket::signal_haveTotalTime,this,&ETCTcpServer::signal_haveTotalTime);
    connect(tcpTemp,&PTcpSocket::signal_haveavgTime,this,&ETCTcpServer::signal_haveavgTime);
    connect(tcpTemp,&PTcpSocket::signal_havesuccessRate,this,&ETCTcpServer::signal_havesuccessRate);
    connect(this,&ETCTcpServer::SIGNAL_upTest,tcpTemp,&PTcpSocket::slot_upTest);
    connect(this,&ETCTcpServer::SIGNAL_downTest,tcpTemp,&PTcpSocket::slot_downTest);
    connect(this,&ETCTcpServer::SIGNAL_stressTest,tcpTemp,&PTcpSocket::slot_stressTest);
    tcpTemp->moveToThread(thread);              //把tcp类移动到新的线程
    thread->start();
    connect(tcpTemp,&PTcpSocket::sockDisConnect,this,&ETCTcpServer::sockDisConnectSlot);//断开连接的处理，从列表移除，并释放断开的Tcpsock
    //发送连接信号


}
ETCTcpServer::ETCTcpServer(QObject *parent) : QTcpServer(parent),d_ptr(new ETCTcpServerPrivate(this))
{
    Q_D(ETCTcpServer);
    qRegisterMetaType<qintptr>("qintptr");
}

QMap<int, PTcpSocket *>* ETCTcpServer::getSocketSInfo()
{
    Q_D(ETCTcpServer);
    return d->tcpClient;
}

ETCTcpServer::~ETCTcpServer()
{}
/**
 * @brief ETCTcpServer::sockDisConnectSlot
 * @param handle
 * @param ip
 * @param port
 * @note 处理客户端退出
 */
void ETCTcpServer::sockDisConnectSlot(qintptr handle, QString ip, quint16 port)
{
    Q_D(ETCTcpServer);
    qDebug()<<"客户端"<<handle<<"已退出";
    int a = d->tcpClient->size() - 1;
    PTcpSocket *tcp = d->tcpClient->value(static_cast<int>(handle));
    d->tcpClient->remove(static_cast<int>(handle));
    emit sockDisConnected(handle,ip,port);
    //连接管理中移除断开连接的socket
    delete tcp;
    tcp = nullptr;
    if(tcp == nullptr && d->tcpClient->size() == a)
    {
        qDebug()<<"客户端"<<handle<<"socket资源释放成功";
    }

}
