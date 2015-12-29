#ifndef COFFEE_EXT_QT_SHIM_QT_NODE_EDITOR_NODE_EDIT_H
#define COFFEE_EXT_QT_SHIM_QT_NODE_EDITOR_NODE_EDIT_H

#include <iostream>

#include <QtCore/QRectF>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsRectItem>

#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QPainter>

namespace Coffee{
namespace CQt{

class CQNodeItem : public QGraphicsRectItem
{
public:
    CQNodeItem():
        QGraphicsRectItem(QRectF(0,0,100,100),0)
    {
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        painter->fillRect(this->boundingRect(),Qt::black);
    }
};

class CQNodeEditor : public QGraphicsScene
{
public:
    void insertNode(CQNodeItem* it)
    {
        this->addItem(it);
    }

protected:
    void drawBackground(QPainter *painter, const QRectF &rect)
    {
        painter->fillRect(rect,Qt::red);
    }
};

}
}

#endif
