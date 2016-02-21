#include <coffee_ext/qt_shim/dialogs/dialogs.h>

#include "customsplash.h"

#include <QImage>
#include <QPixmap>

#include <QDesktopWidget>
#include <QScreen>

namespace CoffeeExt{
namespace QtDialogs{

struct QtSplash::SplashHandle
{
    SplashHandle():
        splash(new CustomSplashScreen)
    {
    }
    ~SplashHandle()
    {
        splash->deleteLater();
    }

    QDesktopWidget w;
    CustomSplashScreen* splash;
    QImage pimg;
    QPixmap pixm;
};

QtSplash::SplashHandle *QtSplash::CreateSplash()
{
    SplashHandle* s = new SplashHandle;
    return s;
}

void QtSplash::ShowSplash(SplashHandle *s)
{
    s->splash->show();
}

void QtSplash::HideSplash(SplashHandle *s)
{
    s->splash->hide();
}

void QtSplash::SetSize(SplashHandle *s, const CSize &size)
{
    s->splash->setFixedSize(size.w,size.h);

    QRect geo = s->w.availableGeometry(s->w.primaryScreen());
    QRect optimal;
    optimal.setLeft(geo.center().x()-optimal.width()/2);
    optimal.setTop(geo.center().y()-optimal.height()/2);
    optimal.setRight(optimal.left()+size.w);
    optimal.setBottom(optimal.top()+size.h);

    s->splash->setGeometry(optimal);
}

void QtSplash::SetTitle(SplashHandle *s, const Title &title)
{
    new(&(s->splash->m_title)) Title(title);
    s->splash->repaint();
}

void QtSplash::SetSubText(SplashHandle *s, const Title &title)
{
    new(&(s->splash->m_subtext)) Title(title);
    s->splash->repaint();
}

void QtSplash::SetProgress(QtSplash::SplashHandle *s, scalar p)
{
    s->splash->percentage = p;
    s->splash->repaint();
}

bool QtSplash::SetBitmap(SplashHandle *s, PixelFormat fmt, const CSize &size, const byte_t *data)
{
    QImage::Format qfmt = QImage::Format_Invalid;

    switch(fmt)
    {
    case PixelFormat::R8UI:
        qfmt = QImage::Format_Mono;
        break;
    case PixelFormat::RGB4UI:
        qfmt = QImage::Format_RGB444;
        break;
    case PixelFormat::RGB5UI:
        qfmt = QImage::Format_RGB555;
        break;
    case PixelFormat::RGB8UI:
        qfmt = QImage::Format_RGB888;
        break;
    case PixelFormat::RGBA8UI:
        qfmt = QImage::Format_RGBA8888;
        break;
    default:
        return false;
    }

    s->pimg = QImage(data,size.w,size.h,qfmt);
    s->splash->setPixmap(QPixmap::fromImage(s->pimg));

    return true;
}

void QtSplash::DestroySplash(SplashHandle *s)
{
    delete s;
}

}
}