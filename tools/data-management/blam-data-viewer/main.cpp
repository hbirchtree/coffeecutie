#include <QApplication>
#include <QWidget>

#include <blamdataviewer.h>

int main(int argc, char** argv)
{
    QApplication app(argc,argv);

    app.setApplicationDisplayName("Blam Data Viewer");
    app.setApplicationName("Blam View");
    app.setApplicationVersion("1.0");

    BlamDataViewer viewer;

    viewer.show();

    return app.exec();
}
