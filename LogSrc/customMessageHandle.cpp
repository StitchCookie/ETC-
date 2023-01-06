#include "LogSrc/customMessageHandle.h"
bool bEnableDebugLog =  0 ; //是否启用日志输出
bool bStressDebugLog =  0 ;
void WriteLine(QString logFileName,  QString logMessage)
{
    QFile file(logFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream.setCodec("utf-8");
    text_stream << logMessage << "\r\n";
    file.flush();
    file.close();
}

/**
  * @brief Log
  * @param logFileName
  * @param logMessage
  * @note  写入文件
  */
 void Log(QString logFileName,  QString logMessage)
{
    static QMutex mutex;
    mutex.lock();

    if (bEnableDebugLog)
    {
         WriteLine(logFileName, logMessage);
    }

    mutex.unlock();
}

 /**
 * @brief outputMessage
 * @param type
 * @param context 消息上下文
 * @param msg     写入的调试信息
 */
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);

    QString message;
    QString text;
    QString fileName;

    switch(type)
    {
    case QtDebugMsg:
       text = QString("Debug:");
       message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

       fileName = (bStressDebugLog) ? "stressLogDebug.txt" : "logDebug.txt";
       Log(fileName,message);
       break;

    case QtWarningMsg:
       text = QString("Warning:");
       message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
       fileName = "logWarning.txt";
       Log(fileName,message);
       break;

    case QtCriticalMsg:
       text = QString("Critical:");
       message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
       fileName = "logCritical.txt";
       Log(fileName,message);
       break;

    case QtFatalMsg:
        text = QString("Fatal:");
        message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
        fileName = "logFatal.txt";
        Log(fileName,message);
        break;
    }
}
