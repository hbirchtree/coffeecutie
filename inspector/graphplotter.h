#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>

class GraphPlotter : public QGraphicsScene
{
public:
    GraphPlotter(QObject *parent);

    void addPlot(float height);

protected:
    QVector<float> values;
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
};

#endif // GRAPHPLOTTER_H
