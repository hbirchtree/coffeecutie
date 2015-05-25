#include "graphplotter.h"

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
    painter->setPen(QPen(QColor(255,0,0,127)));
    painter->drawLine(rect.left(),-100,rect.right(),-100);
    painter->setPen(QPen(QColor(255,0,0,255)));
    painter->drawText(rect.left(),-100,"100.0ms");
    painter->setPen(QPen(QColor(255,0,0,127)));
    painter->drawLine(rect.left(),-33.2,rect.right(),-33.2);
    painter->setPen(QPen(QColor(255,0,0,255)));
    painter->drawText(rect.left(),-33.2,"33.2ms");
    painter->setPen(QPen(QColor(255,255,0,127)));
    painter->drawLine(rect.left(),-16.6,rect.right(),-16.6);
    painter->setPen(QPen(QColor(255,255,0,255)));
    painter->drawText(rect.left(),-16.6,"16.6ms");
    painter->setPen(QPen(QColor(0,0,255,127)));
    painter->drawLine(rect.left(),0,rect.right(),0);
    painter->setPen(QPen(QColor(0,0,255,255)));
    painter->drawText(rect.left(),0,"0.0ms");
}

void GraphPlotter::drawForeground(QPainter *painter, const QRectF &rect)
{
    painter->setPen(QPen(Qt::green));
    float xcrd_inc = (float)rect.width()/(float)values.size();
    float ycrd = -values.first();
    for(int i=0;i<values.size();i++){
        painter->drawLine(rect.left()+xcrd_inc*(float)i,
                          ycrd,
                          rect.left()+xcrd_inc*(float)(i+1),
                          -values.at(i));
        ycrd = -values.at(i);
    }
}
