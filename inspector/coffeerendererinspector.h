#ifndef COFFEERENDERERINSPECTOR_H
#define COFFEERENDERERINSPECTOR_H

#include <QWidget>
#include "opengl/context/coffeerenderer.h"
#include "graphplotter.h"
#include <QPainter>
#include <QTreeWidgetItem>

namespace Ui {
class CoffeeRendererInspector;
}

class CoffeeSystemInformation : public QObject
{
    //Because Qt does not give memory information about our own process
public:
    void updateData();

    double getVirtualMemory() const;
    double getResidentMemory() const;
    quint16 getThreadCount() const;
    qint8 getPriority() const;

private:
    quint64 m_vrt = 0;
    quint64 m_rss = 0;
    quint16 m_thrds = 0;
    qint8 m_priority = 0;
};

class CoffeeRendererInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeRendererInspector(QWidget *parent = 0,CoffeeRenderer* renderer = nullptr);
    ~CoffeeRendererInspector();

private slots:
    void plotGraph(float frametime);
    void on_plotResizer_valueChanged(int value);

    void setSysInfoField(const QString& field, const QString& data);

private:
    CoffeeRenderer* renderer;

    QColor clearColor;
    qint64 checkInterval = 200;
    qint64 measureTime = 0;
    QPointF last;

    qreal readings = 0;
    GraphPlotter* scene = nullptr;
    Ui::CoffeeRendererInspector *ui;

    CoffeeSystemInformation m_sysinfo;

    QHash<QString,QTreeWidgetItem*> m_properties;
};

#endif // COFFEERENDERERINSPECTOR_H
