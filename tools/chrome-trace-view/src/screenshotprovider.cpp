#include "screenshotprovider.h"

#include "tracemodel.h"

#include <QByteArray>
#include <QImage>

ScreenshotProvider::ScreenshotProvider(TraceModel* values, QObject* parent) :
    QQuickImageProvider(ImageType::Image), m_values(values)
{
}

QImage ScreenshotProvider::requestImage(
    const QString& id, QSize* size, const QSize& requestedSize)
{
    quint64 index;
    index = id.toULong();

    auto event = m_values->traceEvents().at(index).toObject();

    auto encoded = event.find("v");

    if(encoded == event.end())
        return {};

    return QImage::fromData(
        QByteArray::fromBase64(encoded->toString().toLocal8Bit()));
}
