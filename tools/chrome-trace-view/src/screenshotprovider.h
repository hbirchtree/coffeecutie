#pragma once

#include <QQuickImageProvider>

class TraceModel;

class ScreenshotProvider : public QQuickImageProvider
{
    TraceModel* m_values;

  public:
    ScreenshotProvider(TraceModel* values, QObject* parent = nullptr);

    QImage requestImage(
        const QString& id, QSize* size, const QSize& requestedSize);
};
