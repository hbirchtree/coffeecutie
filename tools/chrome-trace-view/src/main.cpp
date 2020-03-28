#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "tracemodel.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<TraceModel>("me.birchtrees.ctf", 1, 0, "TraceModel");

    QUrl const url(QStringLiteral("qrc:/main.qml"));

    QQuickStyle::setStyle("Material.Dark");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if(!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
