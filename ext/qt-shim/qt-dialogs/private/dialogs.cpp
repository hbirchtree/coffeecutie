#include <coffee_ext/qt_shim/dialogs/dialogs.h>
#include <QGuiApplication>
#include <QApplication>
#include <QSplashScreen>

#include <QImage>
#include <QPixmap>

namespace CoffeeExt{
namespace QtDialogs{

thread_local QApplication* dialog_application = nullptr;

void QtInitApplication(int &argc, char** argv)
{
    if(!dialog_application)
    {
        dialog_application = new QApplication(argc,argv);
    }
}

void QtExitApplication()
{
    delete dialog_application;
}

void QtProcessEvents(int timeout)
{
    QApplication::processEvents();
}

struct QtSplash::SplashHandle
{
    SplashHandle():
        splash(new QSplashScreen)
    {
    }
    ~SplashHandle()
    {
        splash->deleteLater();
    }

    QSplashScreen* splash;
    QImage pimg;
    QPixmap pixm;
};

QtSplash::SplashHandle *QtSplash::CreateSplash()
{
    SplashHandle* s = new SplashHandle;

    s->splash->setFixedSize(800,600);

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
}

void QtSplash::SetTitle(SplashHandle *s, CString title)
{
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