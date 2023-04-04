#pragma once

#include <QJsonArray>
#include <QJsonObject>

#include <memory>

struct TraceMetaData
{
    std::shared_ptr<QJsonObject> trace;
    std::shared_ptr<QJsonArray> events;

    double timestampDivisor{1.0};
    qint64 firstTimestamp{std::numeric_limits<qint64>::max()};
    qint64 totalDuration{0};
};
