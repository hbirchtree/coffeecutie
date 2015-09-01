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

    int getPlotSize() const;
    void setPlotSize(int value);

protected:
    int plotSize = 10;
    QVector<float> values;
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);

private:
    void drawFrametimeIndicator(QPainter* painter, const QColor &color,
                                const QString& text, float height,
                                const QRectF &rect);

    double m_yscale = 2.0;
};

#endif // GRAPHPLOTTER_H
