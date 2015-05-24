#include "graphplotter.h"

GraphPlotter::GraphPlotter(QObject *parent) : QGraphicsScene(parent)
{

}

void GraphPlotter::addPlot(float height)
{
    values.push_front(height);
    if(values.size()>10){
        values.resize(10);
    }
}

void GraphPlotter::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->setBrush(QBrush(Qt::black,Qt::SolidPattern));
    painter->setPen(QPen(Qt::black));
    painter->drawRect(rect);
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
    float ycrd = values.first();
    for(int i=0;i<values.size();i++){
        painter->drawLine(rect.left()+xcrd_inc*(float)i,
                          ycrd,
                          rect.left()+xcrd_inc*(float)(i+1),
                          -values.at(i));
        ycrd = -values.at(i);
    }
}
