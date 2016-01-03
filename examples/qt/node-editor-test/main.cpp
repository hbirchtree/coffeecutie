#include <coffee/CCore>
#include <node-edit.h>
#include "mainwindowtester.h"

using namespace Coffee;

int32 coffee_main(int32 a, byte_t** c)
{
    QApplication mn(a,c);

    MainWindowTester mainwin;

    Coffee::CQt::CQNodeEditor nedit;

    QGraphicsView dispwidget;
    dispwidget.setDragMode(QGraphicsView::ScrollHandDrag);
    dispwidget.setScene(&nedit);

    mainwin.addWidget(&dispwidget);
    mainwin.show();

    QColor blackGray(40,40,40);

    Coffee::CQt::CQNodeItem nd(QRectF(0,0,200,200),0);
    nd.setContentSize(QSizeF(100,100));
    nd.setText("Node 1");
    nd.setTextColor(Qt::white);
    nd.setBackgroundColor(blackGray);
    nd.setEdgeColor(blackGray);
    nd.setHeaderColor(Qt::lightGray);
    nd.setHeaderSpacing(0.8);
    nd.setPadding(2);
    nd.setSlotRadius(8.0);

    Coffee::CQt::CQNodeItem nd2(QRectF(250,0,200,200),0);
    nd2.setText("Node 2");
    nd2.setTextColor(Qt::white);
    nd2.setBackgroundColor(blackGray);
    nd2.setEdgeColor(blackGray);
    nd2.setHeaderColor(Qt::darkRed);
    nd2.setHeaderSpacing(0.8);
    nd2.setPadding(2);
    nd2.setSlotRadius(8.0);

    Coffee::CQt::CQNodeItem::CQNodeConnection cnn0("Test slot",true,&nd2);
    cnn0.setIndex(0);
    Coffee::CQt::CQNodeItem::CQNodeConnection cnn1("Test signal",false,&nd2);
    cnn1.setIndex(0);

    Coffee::CQt::CQNodeItem::CQNodeConnection cnn2("Test slot?",true,&nd);
    cnn2.setIndex(0);

    Coffee::CQt::CQNodeItem::CQNodeCurve line(&cnn1,&cnn2,0);

    nedit.addItem(&nd);
    nedit.addItem(&nd2);
    nedit.addItem(&line);

    return mn.exec();
}

COFFEE_APPLICATION_MAIN(coffee_main)
