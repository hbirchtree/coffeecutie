#ifndef COFFEELOGGER_H
#define COFFEELOGGER_H

#include <QMessageLogContext>
#include <QDateTime>
#include <QFile>

#include <QDebug>

class CoffeeLogger : public QObject
{
public:
    CoffeeLogger(bool logStdOut = true, bool logFile = false);
    ~CoffeeLogger();

    static void defaultMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg);
private:
    static QFile* outputFile;
    static bool logStdOut;
    static bool logFile;
};

#endif // COFFEELOGGER_H
