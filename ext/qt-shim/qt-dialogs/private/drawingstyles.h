#include <coffee/core/coffee_mem_macros.h>

#include <QFontMetrics>
#include <QPainter>

namespace CoffeeExt{
namespace QtDialogs{

struct DrawingStyleDef
{
    static void DrawTitle(QPainter* painter,
                          QRect const& win,
                          QtSplash::Title const& title,
                          int prog_height);
    static void DrawProgress(QPainter* painter,
                             QRect const& win,
                             QColor const& prog_color,
                             float progress,
                             int height);
    static void DrawBackdrop(QPainter* painter,
                             QRect const& win,
                             QColor const& baseColor);
};

struct SimpleDrawingStyle : DrawingStyleDef
{
    STATICINLINE void DrawTitle(QPainter* painter,
                                QRect const& win,
                                QtSplash::Title const& title,
                                int prog_height)
    {
        Qt::Alignment a = qt_alignflag(title.m_align);

        QPen p;
        p.setColor(qt_colorflag(title.m_col));

        painter->setPen(p);

        QFont t_fn = painter->font();
        QFont fn = painter->font();
        fn.setBold(true);
        fn.setPointSize(title.m_size);
        fn.setKerning(true);

        painter->setFont(fn);

        QString text = QString::fromStdString(title.m_title);

        QRect r = QFontMetrics(fn).boundingRect(text);

        r.moveLeft(win.left());
        r.moveTop(win.top());

        if(a&Qt::AlignRight)
            r.moveRight(win.right());

        if(a&Qt::AlignVCenter)
            r.moveBottom(win.center().y()-r.height()/2);
        if(a&Qt::AlignHCenter)
            r.moveLeft(win.center().x()-r.width()/2);

        if(a&Qt::AlignBottom)
            r.moveBottom(win.bottom()-prog_height);

        painter->drawText(r,text);

        painter->setFont(t_fn);
    }
    STATICINLINE void DrawProgress(QPainter* painter,
                                   QRect const& win,
                                   QColor const& color,
                                   float progress,
                                   int height)
    {
        QRectF bar(win.left(),win.bottom()-height,win.width(),height);
        bar.setWidth(bar.width()*progress);
        painter->setPen(Qt::transparent);

        QPoint tl = win.topLeft();
        QPoint bl = win.bottomLeft();

        tl.setY(win.bottom()-height);

        QLinearGradient grad(tl,bl);
        grad.setColorAt(0.0,color);
        grad.setColorAt(1.0,color.dark());

        QBrush brush(grad);

        painter->setBrush(brush);
        painter->drawRect(bar);
    }
    STATICINLINE void DrawBackdrop(QPainter* painter,
                                   QRect const& win,
                                   QColor const& baseColor)
    {
        painter->setPen(Qt::transparent);

        /*Background gradient*/
        {
            QRadialGradient sunshine(win.topRight(),win.center().rx()*2);
            sunshine.setColorAt(0.0,baseColor.darker());
            sunshine.setColorAt(1.0,baseColor.darker().darker().darker());

            QBrush brush(sunshine);
            painter->setBrush(brush);

            painter->drawRect(win);
        }
        /*Prominent circle*/
        {
            QBrush brush(baseColor);
            painter->setBrush(brush);

            int radius = std::max(win.center().rx(),win.center().ry())*5/6;

            painter->drawEllipse(win.bottomLeft(),radius,radius);
            painter->drawEllipse(win.topLeft(),radius/2,radius/2);
            painter->drawEllipse(win.bottomRight(),radius*2,radius*2);
            painter->drawEllipse(win.topRight(),radius*4,radius*4);
        }
    }
};

using DrawingStyle = SimpleDrawingStyle;

}
}