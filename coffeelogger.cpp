#include "coffeelogger.h"

CoffeeLogger::CoffeeLogger()
{
    qInstallMessageHandler(&defaultMessageHandler);
}

void CoffeeLogger::defaultMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg)
{
    QByteArray m_msg = msg.toLocal8Bit();
    switch(t){
    case QtDebugMsg:
        fprintf(stderr, "DEBUG:%s:%u: %s\n",context.function,context.line,m_msg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "WARN:%s:%u: %s\n",context.function,context.line,m_msg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "CRITICAL:%s:%u: %s\n",context.function,context.line,m_msg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "FATAL:%s:%u: %s\n",context.function,context.line,m_msg.constData());
        break;
    default:
        fprintf(stderr, "???:%s:%u: %s\n",context.function,context.line,m_msg.constData());
    }
}
