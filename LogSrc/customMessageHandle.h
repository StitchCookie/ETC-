#ifndef CUSTOMMESSAGEHANDLE_H
#define CUSTOMMESSAGEHANDLE_H
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include<QMutex>
#include<QDateTime>
#include<QFile>
#include<QTextStream>

#include<QFile>
#include <QDebug>
#include<QMessageBox>

/**
* @projectName JplocalSense
* @brief 摘要 :日志输出
*    调用格式如下：
*     qDebug("This is a debug message");
*     qWarning("This is a warning message");
*     qCritical("This is a critical message");
*     qFatal("This is a fatal message");
*
* @date 2020-10-30
*/

extern bool bEnableDebugLog; //是否启用日志输出
extern bool bStressDebugLog; //是否为压力测试日志输出
void WriteLine(QString logFileName,  QString logMessage);


void Log(QString logFileName,  QString logMessage);

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);


#endif // CUSTOMMESSAGEHANDLE_H
