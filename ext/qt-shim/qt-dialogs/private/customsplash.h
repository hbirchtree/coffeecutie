#include <coffee/core/display/cdialog_fun.h>

#include "conversion.h"
#include "drawingstyles.h"

#include <QSplashScreen>
#include <QPainter>

namespace CoffeeExt{
namespace QtDialogs{

class CustomSplashScreen : public QSplashScreen
{
public:
    CustomSplashScreen():
        QSplashScreen(0),
        m_subtext(""),
        m_title("")
    {
        setCursor(Qt::WaitCursor);
    }
    QtSplash::Title m_subtext;
    QtSplash::Title m_title;
    float percentage = 0;

protected:
    FORCEDINLINE void mousePressEvent(QMouseEvent *)
    {
    }
    FORCEDINLINE void drawContents(QPainter *painter)
    {
        /* Parameters */
        QFont base_font = painter->font();
        QRect space = this->contentsRect();
        int prog_height = base_font.pointSize();
        QColor prog_color = QColor(255,127,0);
        QColor base_color = QColor(255,127,0,70).dark();

        /*HACKY HACKY*/
        QPainter::CompositionMode cmode = painter->compositionMode();
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->fillRect(space,Qt::transparent);
        painter->setCompositionMode(cmode);
        /**/

        DrawingStyle::DrawBackdrop(painter,space,base_color);

        painter->drawPixmap(space,pixmap());

        /* Draw titles */
        DrawingStyle::DrawTitle(painter,space,m_title,prog_height+1);
        DrawingStyle::DrawTitle(painter,space,m_subtext,prog_height+1);

        painter->setFont(base_font);

        DrawingStyle::DrawProgress(painter,space,prog_color,
                                   percentage,prog_height);
    }
};

}
}