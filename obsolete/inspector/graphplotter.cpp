#include "graphplotter.h"

#include <QDebug>

#define BOTTOM_OFFSET 5.0

GraphPlotter::GraphPlotter(QObject *parent) : QGraphicsScene(parent)
{

}

void GraphPlotter::addPlot(float height)
{
    values.push_front(height);
    if(values.size()>plotSize){
        values.resize(plotSize);
    }
}
int GraphPlotter::getPlotSize() const
{
    return plotSize;
}

void GraphPlotter::setPlotSize(int value)
{
    plotSize = value;
}


void GraphPlotter::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->setBrush(QBrush(Qt::black,Qt::SolidPattern));
//    painter->setPen(QPen(Qt::black));
    painter->drawRect(rect);

    //time marks
    painter->setPen(QPen(QColor(0,255,0,70)));
    for(int i=1;i<plotSize;i++){
        qreal dist = (qreal)i*rect.width()/(qreal)plotSize-rect.width()/2.f;
        painter->drawLine(dist,rect.top(),dist,rect.bottom());
    }
    painter->setPen(QPen(QColor(0,255,0,35)));
    for(int i=1;i<plotSize*4;i++){
        qreal dist = (qreal)i*rect.width()/(qreal)(plotSize*4)-rect.width()/2.f;
        painter->drawLine(dist,rect.top(),dist,rect.bottom());
    }

    //millisecond marks, 100, 33.2, 16.6, 0
    double bottomOffset = -BOTTOM_OFFSET;

    drawFrametimeIndicator(painter,QColor(255,0,0,255),
                           "100.0ms",
                           rect.bottom()-100.0*m_yscale+bottomOffset,rect);
    drawFrametimeIndicator(painter,QColor(255,0,0,255),
                           "33.2ms",
                           rect.bottom()-33.2*m_yscale+bottomOffset,rect);
    drawFrametimeIndicator(painter,QColor(255,255,0,255),
                           "16.6ms",
                           rect.bottom()-16.6*m_yscale+bottomOffset,rect);
    drawFrametimeIndicator(painter,QColor(0,0,255,255),
                           "0.0ms",
                           rect.bottom()+bottomOffset,rect);
}

void GraphPlotter::drawForeground(QPainter *painter, const QRectF &rect)
{
    if(values.size()==0)
        return;
    painter->setPen(QPen(Qt::green));

    float xcrd_inc = (float)rect.width()/(float)values.size();
    float ycrd = -values.first()*m_yscale+(float)rect.bottom()-BOTTOM_OFFSET;
    for(int i=0;i<values.size();i++){
        float _ycrd = -values.at(i)*m_yscale+(float)rect.bottom()-BOTTOM_OFFSET;
        painter->drawLine(rect.left()+xcrd_inc*(float)i,
                          ycrd,
                          rect.left()+xcrd_inc*(float)(i+1),
                          _ycrd);
        ycrd = _ycrd;
    }
}

void GraphPlotter::drawFrametimeIndicator(QPainter *painter, const QColor& color,
                                          const QString &text, float height,
                                          const QRectF& rect)
{
    //Draw the line
    QColor lineColor(color);
    lineColor.setAlphaF(lineColor.alphaF()/2.0); //Line is more transparent
    painter->setPen(QPen(lineColor));
    painter->drawLine(rect.left(),height,rect.right(),height);

    //Draw the label
    painter->setPen(QPen(color));
    painter->drawText(rect.left(),height,text);
}
