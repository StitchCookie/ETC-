#ifndef PMQTT_H
#define PMQTT_H

#include <QObject>
#include <qmqttclient.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
class Pmqtt : public QMqttClient
{
    Q_OBJECT
public:
    explicit Pmqtt(QObject *parent = nullptr);
    void pckMessage(QString topicName,quint8 state );
signals:
    void SIGNAL_Date(const QByteArray,QString type,QString topicName);
    void SIGNAL_State(QString topicName,const QByteArray &message);
public slots:
    void SLOT_pauseMessage(const QByteArray &message, const QMqttTopicName &topic);
};

#endif // PMQTT_H
