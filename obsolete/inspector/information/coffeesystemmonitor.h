#ifndef COFFEESYSTEMMONITOR_H
#define COFFEESYSTEMMONITOR_H

#include <QObject>

class CoffeeRenderer;

class CoffeeSystemMonitor : public QObject
{
    Q_OBJECT
public:
    CoffeeSystemMonitor(QObject *parent);

    void updateData();

    Q_INVOKABLE double getVirtualMemory() const;
    Q_INVOKABLE double getResidentMemory() const;
    Q_INVOKABLE quint16 getThreadCount() const;
    Q_INVOKABLE qint16 getPriority() const;

    Q_INVOKABLE QString interpretVideoMemory(CoffeeRenderer* ren);

public slots:
    void setRenderer(CoffeeRenderer* renderer);
    void refresh();

signals:
    void displayInformation(const QString& field,const QString& data);

private:
    CoffeeRenderer* m_renderer = nullptr;

    quint64 m_vrt = 0;
    quint64 m_rss = 0;
    quint16 m_thrds = 0;
    qint8 m_priority = 0;
};

#endif // COFFEESYSTEMMONITOR_H
