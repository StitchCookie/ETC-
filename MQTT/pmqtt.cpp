#include "pmqtt.h"

Pmqtt::Pmqtt(QObject *parent) : QMqttClient(parent)
{
    connect(this, &QMqttClient::messageReceived, this, &Pmqtt::SLOT_pauseMessage);
}
/**
 * @brief 通用接口发布
 * @param 发布主题
 * @param 设备状态
 */
void Pmqtt::pckMessage(QString topic, quint8 state)
{

    QStringList list = topic.split("/");
    QString currDev = list.at(2);
    QString currNum = list.at(3);
    QDateTime curTime = QDateTime::currentDateTime();
    QString Time = curTime.toString("yyyyMMddhh");
    QString Id = QString("%1%2%3012345").arg(Time).arg(currDev).arg(currNum);
    QJsonArray array;
    array.append(Id);
    array.append("1");
    array.append(QString("%1").arg(state));
    array.append(QString("00000001"));
    QJsonDocument joc(array);
    QByteArray message = joc.toJson();
    publish(topic,message);
}
/**
 * @brief 解析订阅消息
 * @param message
 * @param topic
 */
void Pmqtt::SLOT_pauseMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    QString topicName = topic.name();
    QStringList topicList = topicName.split("/");
    QString curTopicName = topicList.at(5);
    if(curTopicName.compare("display") == 0)
    {

        emit SIGNAL_Date(message,"display",topicName);
    }else if(curTopicName.compare("audio") == 0)
    {
        emit SIGNAL_Date(message,"audio",topicName);
    }else if(curTopicName.compare("alarm") == 0)
    {
        emit SIGNAL_Date(message,"alarm",topicName);
    }else if(curTopicName.compare("passlight") == 0)
    {
        emit SIGNAL_Date(message,"passlight",topicName);
    }else if(curTopicName.compare("lanelight") == 0)
    {
        emit SIGNAL_Date(message,"lanelight",topicName);
    }else if(curTopicName.compare("board") == 0)
    {
        emit SIGNAL_Date(message,"board",topicName);
    }else if(curTopicName.compare("monitor") == 0)
    {

    }else if(topicList.at(7).compare("DevInfo") == 0)
    {

    }

}

