#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_deviceServer.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include "DataStruct.h"
#include <QHostAddress>
#include <QTimer>
#include <QDateTime>
#include "TCP_PUBLIC/PTcpSocket.h"
#include "TCP_PUBLIC/mytcpserver.h"
class deviceServer : public QMainWindow
{
    Q_OBJECT

public:
    deviceServer(QWidget *parent = nullptr);
    quint16 calc_crc16(quint16 init_crc, quint8* crc_data, int len);
    void resizeEvent(QResizeEvent *EVENT);
    void initWgt();
    ~deviceServer();
private slots :
    void SLOT_serVerListen();
    void on_makeReport_clicked();
    void on_heartStatus_clicked();

private:
    Ui::deviceServerClass			ui;
    MyTcpServer						*m_Tcp_Server;
    QTimer                          *m_TcpServerTimer;
    QLabel                          *m_ConnectStatus;
    bool                             m_heart;
    static quint8                    controlCnt;
    QMap<int,PTcpSocket *>          *Sockets;
    int                             handle;
};
