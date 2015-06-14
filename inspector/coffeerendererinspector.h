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

class CoffeeRendererInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeRendererInspector(QWidget *parent = 0,CoffeeRenderer* renderer = nullptr);
    ~CoffeeRendererInspector();

private slots:
    void plotGraph(float frametime);
    void on_plotResizer_valueChanged(int value);

private:
    CoffeeRenderer* renderer;

    QColor clearColor;
    qint64 checkInterval = 200;
    qint64 measureTime = 0;
    QPointF last;
    QTreeWidgetItem* fpsItem;
    QTreeWidgetItem* frameTimeItem;
    QTreeWidgetItem* memoryUsageItem;
    QTreeWidgetItem* vmemUsageItem;

    qreal readings = 0;
    GraphPlotter* scene = nullptr;
    Ui::CoffeeRendererInspector *ui;
};

#endif // COFFEERENDERERINSPECTOR_H
