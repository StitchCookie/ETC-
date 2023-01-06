#include "mytcpserver.h"

/**
 * @brief MyTcpServer::incomingConnection
 * @param socketDescriptor
 * @note 重新incoming虚函数 实现多线程服务器处理客户端
 */

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    PTcpSocket *tcpTemp = new PTcpSocket(socketDescriptor);
    QThread *thread = new QThread(tcpTemp);
    tcpClient->insert(socketDescriptor,tcpTemp);//插入到连接信息中
     emit connectClient(tcpTemp->socketDescriptor(),tcpTemp->peerAddress().toString(),tcpTemp->peerPort());
    connect(tcpTemp,&PTcpSocket::disconnected,thread,[=](){thread->quit();});//断开连接时线程退出
    connect(tcpTemp,&PTcpSocket::signal_TowCount,this,&MyTcpServer::signal_TowCount);
    connect(tcpTemp,&PTcpSocket::signal_haveVehInfo,this,&MyTcpServer::signal_haveVehInfo);
    connect(tcpTemp,&PTcpSocket::SIGNAL_haveUpTableItem,this,&MyTcpServer::signal_haveUpTableItem,Qt::UniqueConnection);
    connect(tcpTemp,&PTcpSocket::SIGNAL_haveDownTableItem,this,&MyTcpServer::signal_haveDownTableItem,Qt::UniqueConnection);
    connect(tcpTemp,&PTcpSocket::signal_haveTotalTime,this,&MyTcpServer::signal_haveTotalTime);
    connect(tcpTemp,&PTcpSocket::signal_haveavgTime,this,&MyTcpServer::signal_haveavgTime);
    connect(tcpTemp,&PTcpSocket::signal_havesuccessRate,this,&MyTcpServer::signal_havesuccessRate);
    connect(this,&MyTcpServer::SIGNAL_upTest,tcpTemp,&PTcpSocket::slot_upTest);
    connect(this,&MyTcpServer::SIGNAL_downTest,tcpTemp,&PTcpSocket::slot_downTest);
    connect(this,&MyTcpServer::SIGNAL_stressTest,tcpTemp,&PTcpSocket::slot_stressTest);
    tcpTemp->moveToThread(thread);              //把tcp类移动到新的线程
    thread->start();
    connect(tcpTemp,&PTcpSocket::sockDisConnect,this,&MyTcpServer::sockDisConnectSlot);//断开连接的处理，从列表移除，并释放断开的Tcpsock
    //发送连接信号


}
MyTcpServer::MyTcpServer(QObject *parent) : QTcpServer(parent)
{
    tcpClient = new QMap<int,PTcpSocket*>;
}

QMap<int, PTcpSocket *>* MyTcpServer::getSocketSInfo()
{
    return tcpClient;
}

MyTcpServer::~MyTcpServer()
{}
/**
 * @brief MyTcpServer::sockDisConnectSlot
 * @param handle
 * @param ip
 * @param port
 * @note 处理客户端退出
 */
void MyTcpServer::sockDisConnectSlot(qintptr handle, QString ip, quint16 port)
{

    qDebug()<<"客户端"<<handle<<"已退出";
    int a = tcpClient->size() - 1;
    PTcpSocket *tcp = tcpClient->value(static_cast<int>(handle));
    tcpClient->remove(static_cast<int>(handle));
    emit sockDisConnected(handle,ip,port);
    //连接管理中移除断开连接的socket
    delete tcp;
    tcp = nullptr;
    if(tcp == nullptr && tcpClient->size() == a)
    {
        qDebug()<<"客户端"<<handle<<"socket资源释放成功";
    }

}
