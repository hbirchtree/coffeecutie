#include "coffeelogger.h"

CoffeeLogger::CoffeeLogger()
{
    qInstallMessageHandler(&defaultMessageHandler);
}

void CoffeeLogger::defaultMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg)
{
    std::string logtime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString();
    QByteArray m_msg = msg.toLocal8Bit();
    switch(t){
    case QtDebugMsg:
        fprintf(stderr, "%s : DEBUG:%s:%u: %s\n",logtime.c_str(),context.function,context.line,m_msg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s : WARN:%s:%u: %s\n",logtime.c_str(),context.function,context.line,m_msg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s : CRITICAL:%s:%u: %s\n",logtime.c_str(),context.function,context.line,m_msg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s : FATAL:%s:%u: %s\n",logtime.c_str(),context.function,context.line,m_msg.constData());
        break;
    default:
        fprintf(stderr, "%s : ???:%s:%u: %s\n",logtime.c_str(),context.function,context.line,m_msg.constData());
    }
}
