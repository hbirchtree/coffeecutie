#ifndef COFFEELOGGER_H
#define COFFEELOGGER_H

#include <stdio.h>
#include <QString>
#include <QMessageLogContext>
#include <QDateTime>
class CoffeeLogger
{
public:
    CoffeeLogger();
    static void defaultMessageHandler(QtMsgType t, const QMessageLogContext &context, const QString &msg);
};

#endif // COFFEELOGGER_H
