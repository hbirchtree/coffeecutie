#include "coffeelogger.h"

QFile *CoffeeLogger::outputFile;
bool CoffeeLogger::logFile;
bool CoffeeLogger::logStdOut;

CoffeeLogger::CoffeeLogger(bool logStdOut,bool logFile)
{
    this->logStdOut = logStdOut;
    this->logFile = logFile;
    if(logFile){
        outputFile = new QFile("coffee.log",this);
        if(!outputFile->open(QIODevice::Append|QIODevice::WriteOnly|QIODevice::Text)){
            fprintf(stderr,"Failed to create logfile");
            this->logFile = false;
        }
    }
    qInstallMessageHandler(&defaultMessageHandler);
}

CoffeeLogger::~CoffeeLogger()
{
    if(logFile)
        outputFile->close();
}

void CoffeeLogger::defaultMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg)
{
    QString logmessage = QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+":";
    QByteArray m_msg = msg.toLocal8Bit();
    switch(t){
    case QtDebugMsg:
        logmessage.append("DEBUG");
        break;
    case QtWarningMsg:
        logmessage.append("WARN");
        break;
    case QtCriticalMsg:
        logmessage.append("CRITICAL");
        break;
    case QtFatalMsg:
        logmessage.append("FATAL");
        break;
    default:
        logmessage.append("???");
    }

    logmessage.append(QString(":%1:%2: %3\n").arg(context.function).arg(context.line).arg(m_msg.constData()));

    std::string os = logmessage.toStdString();
    if(logFile)
        outputFile->write(os.c_str(),logmessage.size());
    if(logStdOut)
        fprintf(stderr, "%s",
                os.c_str());
}
