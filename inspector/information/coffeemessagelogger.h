#ifndef COFFEEMESSAGELOGGER_H
#define COFFEEMESSAGELOGGER_H

#include <QWidget>

class QTextEdit;

namespace Ui {
class CoffeeMessageLogger;
}

class CoffeeMessageLogger : public QWidget
{
    Q_OBJECT

public:
    explicit CoffeeMessageLogger(QWidget *parent = 0);
    ~CoffeeMessageLogger();
    Q_INVOKABLE int addMessageTab(const QString& title);

public slots:
    void submitMessage(int cat, const QString& text);

private:
    qint64 m_nextMessage = 0;
    QList<QTextEdit*> m_tabs;
    Ui::CoffeeMessageLogger *ui;
};

#endif // COFFEEMESSAGELOGGER_H
