#include <coffee/CCore>
#include <node-edit.h>
#include "mainwindowtester.h"

int32 coffee_main(int32 a, byte_t** c)
{
    QApplication mn(a,c);
    mn.aboutQt();

    MainWindowTester mainwin;

    Coffee::CQt::CQNodeEditor nedit;

    QGraphicsView dispwidget;
    dispwidget.setDragMode(QGraphicsView::ScrollHandDrag);
    dispwidget.setScene(&nedit);

    mainwin.addWidget(&dispwidget);
    mainwin.show();

    Coffee::CQt::CQNodeItem nd;
    nedit.addItem(&nd);

    {
        QFont t;
        t.setPointSize(100);

        nedit.addSimpleText(QString("TestText"),t);
    }

    {
        QLineF t;
        t.setPoints(QPointF(0,0),QPointF(100,100));
        QPen p;
        p.setWidth(4);

        nedit.addLine(t,p);
    }

    return mn.exec();
}

COFFEE_APPLICATION_MAIN(coffee_main)
