#include "deviceServer.h"
#include <QListWidgetItem>
#include "LogSrc/customMessageHandle.h"
quint8 deviceServer::controlCnt = 0;

deviceServer::deviceServer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_Tcp_Server = new ETCTcpServer(this);
    Sockets = m_Tcp_Server->getSocketSInfo();
    m_TcpServerTimer = new QTimer(this);
    bEnableDebugLog = 0;
    bStressDebugLog = 0;
    if(bEnableDebugLog)
    {
        qInstallMessageHandler(outputMessage);
    }
    qRegisterMetaType<qintptr>("qintptr");
    initWgt();
    connect(ui.up,&QPushButton::clicked,m_Tcp_Server,[=](){
        emit m_Tcp_Server->SIGNAL_upTest(ui.comboBox->currentText().toInt());
    });
    connect(ui.down,&QPushButton::clicked,m_Tcp_Server,
            [=](){
        emit m_Tcp_Server->SIGNAL_downTest(ui.comboBox->currentText().toInt());
    });
    connect(ui.stressTest,&QPushButton::clicked,m_Tcp_Server,
            [=](){
        emit m_Tcp_Server->SIGNAL_stressTest(ui.comboBox->currentText().toInt());

    });
    connect(ui.ServerListen, &QPushButton::clicked, this, &deviceServer::SLOT_serVerListen);

    handle = 0;
    connect(m_Tcp_Server,&ETCTcpServer::connectClient,this,[=](qintptr socketDescript,QString address,unsigned short port){
        ui.comboBox->addItem(QString("%1").arg(socketDescript),socketDescript);
        m_ConnectStatus->setPixmap(QPixmap(":/Image/image/connect/Connection.svg"));
        QMessageBox::information(this,"info",QString("新的客户端连接成功：--描述符:%1").arg(socketDescript));
        ui.vehCount->setText(QString::number(0));
        int count = Sockets->size();
        ui.client->setText(QString::number(count,10));

        connect(m_Tcp_Server,&ETCTcpServer::signal_TowCount,this,[=](int socketDescript,int TowCount){
            QMap<int,PTcpSocket*>::iterator iter = Sockets->begin();
            if( iter.key() == socketDescript){
                ui.raiseWgt->setRowCount(TowCount);
            }else{
                ui.raiseWgt_2->setRowCount(TowCount);
            }

            controlCnt = TowCount;
        },Qt::UniqueConnection);

        connect(m_Tcp_Server,&ETCTcpServer::signal_haveVehInfo,this,[=](int,QString vehInfo,int vechicount){

            ui.status->setText(vehInfo);
            ui.vehCount->setText(QString::number(vechicount,10));
        },Qt::UniqueConnection);

        connect(Sockets->value(socketDescript),&PTcpSocket::SIGNAL_haveUpTableItem,this,[=](int handle,QString item1,QString item2,QString item3){
            quint8 index = controlCnt - 1;
            QMap<int,PTcpSocket*>::iterator ite = Sockets->begin();
            if( ite.key() == handle)
            {
                QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(handle,10));
                QTableWidgetItem *it1 = new QTableWidgetItem(item1);
                QTableWidgetItem *it2 = new QTableWidgetItem(item2);
                QTableWidgetItem *it3 = new QTableWidgetItem(item3);
                ui.raiseWgt->setItem(index,0,item0);
                ui.raiseWgt->setItem(index,1,it1);
                ui.raiseWgt->setItem(index,2,it2);
                ui.raiseWgt->setItem(index,3,it3);
            }else{
                QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(handle,10));
                QTableWidgetItem *it1 = new QTableWidgetItem(item1);
                QTableWidgetItem *it2 = new QTableWidgetItem(item2);
                QTableWidgetItem *it3 = new QTableWidgetItem(item3);
                ui.raiseWgt_2->setItem(index,0,item0);
                ui.raiseWgt_2->setItem(index,1,it1);
                ui.raiseWgt_2->setItem(index,2,it2);
                ui.raiseWgt_2->setItem(index,3,it3);
            }
        },Qt::UniqueConnection);

        connect(Sockets->value(socketDescript),&PTcpSocket::SIGNAL_haveDownTableItem,this,[=](int handle,QString item1,QString item2,QString item3){
            quint8 index = controlCnt - 1;
            QMap<int,PTcpSocket*>::iterator iter = Sockets->begin();
            if( iter.key() == handle)
            {
                QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(handle,10));
                QTableWidgetItem *it1 = new QTableWidgetItem(item1);
                QTableWidgetItem *it2 = new QTableWidgetItem(item2);
                QTableWidgetItem *it3 = new QTableWidgetItem(item3);
                ui.raiseWgt->setItem(index,0,item0);
                ui.raiseWgt->setItem(index,1,it1);
                ui.raiseWgt->setItem(index,2,it2);
                ui.raiseWgt->setItem(index,3,it3);
            }else{
                QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(handle,10));
                QTableWidgetItem *it1 = new QTableWidgetItem(item1);
                QTableWidgetItem *it2 = new QTableWidgetItem(item2);
                QTableWidgetItem *it3 = new QTableWidgetItem(item3);
                ui.raiseWgt_2->setItem(index,0,item0);
                ui.raiseWgt_2->setItem(index,1,it1);
                ui.raiseWgt_2->setItem(index,2,it2);
                ui.raiseWgt_2->setItem(index,3,it3);
            }
        },Qt::UniqueConnection);

        connect(m_Tcp_Server,&ETCTcpServer::signal_haveTotalTime,this,[=](int handle,QString Info){
            ui.totalTime->setText(Info);
        });

        connect(m_Tcp_Server,&ETCTcpServer::signal_haveavgTime,this,[=](int handle,QString Info){
            ui.avgTime->setText(Info);
        });

        connect(m_Tcp_Server,&ETCTcpServer::signal_havesuccessRate,this,[=](int handle,QString Info){
            ui.successRate->setText(Info);
        });

    },Qt::UniqueConnection);

    connect(m_Tcp_Server,&ETCTcpServer::sockDisConnected,this,[=](qintptr socketDescript,QString address,unsigned short port){
        ui.client->setText(QString::number(Sockets->size(),10));
        QMessageBox::information(this,"info",QString("有客户端退出连接：--描述符:%1，剩余客户端连接数:%2").arg(socketDescript).arg(QString::number(Sockets->size())));
        QString str = QString("%1").arg(socketDescript);
        int index = ui.comboBox->findData(str);
        qDebug()<<"下标"<<index;
        ui.comboBox->removeItem(index);
        if(Sockets->size() == 0)
        {
            m_ConnectStatus->setPixmap(QPixmap(":/Image/image/connect/Disconnect.svg"));
        }
    });


}

quint16 deviceServer::calc_crc16(quint16 init_crc, quint8* crc_data, int len)
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
    return (quint16)crc;
}

void deviceServer::resizeEvent(QResizeEvent *EVENT)
{
    //qDebug()<<this->size();
}

/**
 * @brief deviceServer::initWgt
 * @note 初始化界面
 */
void deviceServer::initWgt()
{
    m_heart =true;
    setFixedSize(640,590);
    QStringList headerList;
    headerList<<"客户端句柄"<<"控制指令"<<"结果"<<"耗时";
    ui.raiseWgt->setColumnCount(headerList.count());
    ui.raiseWgt->setHorizontalHeaderLabels(headerList);
    ui.raiseWgt->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //ui.raiseWgt->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
    ui.raiseWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.raiseWgt_2->setColumnCount(headerList.count());
    ui.raiseWgt_2->setHorizontalHeaderLabels(headerList);
    ui.raiseWgt_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui.raiseWgt_2->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
    ui.raiseWgt_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statusBar()->setMinimumHeight(25);
    QLabel *label = new QLabel(this);
    label->setText("连接状态");
    ui.statusBar->addWidget(label);
    m_ConnectStatus = new QLabel(this);
    m_ConnectStatus->setScaledContents(true);
    m_ConnectStatus->setMaximumSize(16, 16);
    m_ConnectStatus->setPixmap(QPixmap(":/Image/image/connect/Disconnect.svg"));
    ui.statusBar->addWidget(m_ConnectStatus);
}

deviceServer::~deviceServer()
{
}

void deviceServer::SLOT_serVerListen()
{
    unsigned short port = ui.Port->text().toUShort();
    m_Tcp_Server->listen(QHostAddress::Any, port);
    ui.ServerListen->setEnabled(false);
}


void deviceServer::on_makeReport_clicked()
{

}
/**
 * @brief deviceServer::on_heartStatus_clicked
 * @note 心跳恢复/暂停设置
 */
void deviceServer::on_heartStatus_clicked()
{
    //TODO


    //    if(m_heart == true)
    //    {
    //        int  currHandle =  ui.comboBox->currentData().toInt();
    //        Sockets->value(currHandle)->stopHeart();
    //        ui.heartStatus->setText("恢复心跳");
    //        m_heart = false;
    //    }else{
    //        int  currHandle =  ui.comboBox->currentData().toInt();
    //        Sockets->value(currHandle)->startHeart();
    //        ui.heartStatus->setText("暂停心跳");
    //        m_heart = true;
    //    }
}
