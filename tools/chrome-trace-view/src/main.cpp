#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "screenshotprovider.h"
#include "tracemodel.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    auto context    = engine.rootContext();
    auto traceModel = new TraceModel(&app);

    context->setContextProperty("processes", traceModel);
    context->setContextProperty("traceMeta", traceModel->meta());
    engine.addImageProvider(
        "screenshot", new ScreenshotProvider(traceModel, &app));

    QUrl const url(QStringLiteral("qrc:/main.qml"));

#if !defined(__EMSCRIPTEN__)
    QQuickStyle::setStyle("Material.Dark");
    context->setContextProperty("isEmscripten", false);
#else
    context->setContextProperty("isEmscripten", true);
#endif

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
