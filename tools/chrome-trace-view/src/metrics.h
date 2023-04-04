#pragma once

#include <QAbstractListModel>
#include <QByteArray>
#include <QModelIndex>
#include <QVariant>

#include <memory>

#include "shared_fields.h"
#include "tracemetadata.h"

class Metrics;
class ScreenshotProvider;

class MetricValues : public QAbstractListModel
{
    friend class ScreenshotProvider;

    Q_OBJECT

    Q_PROPERTY(double timestamp READ timestamp)
    Q_PROPERTY(double duration READ duration)

    Q_PROPERTY(double average MEMBER m_average)
    Q_PROPERTY(float max MEMBER m_maxValue)
    Q_PROPERTY(float min MEMBER m_minValue)
    Q_PROPERTY(quint64 numEvents MEMBER m_numEvents)

    friend class Metrics;

    struct Value
    {
        quint64  ts;
        float   value;
        quint64 i;
    };

    TraceMetaData&     m_meta;
    std::vector<Value> m_values;

    quint64 m_numEvents = 0;
    float   m_minValue  = std::numeric_limits<float>::max(),
          m_maxValue    = std::numeric_limits<float>::min();
    double m_average    = 0.f;
    double m_prevTime   = 0.f;

  public:
    MetricValues(Metrics* metrics, TraceMetaData& meta, QObject* parent = nullptr);

    enum FieldNames
    {
        FieldValue,
        FieldIndex,
        FieldValueScaled,
        FieldPreviousValue,
        FieldPreviousValueScaled,
        FieldTimestamp         = TIMESTAMP_ROLE,
        FieldPreviousTimestamp = PREV_TIMESTAMP_ROLE,
    };

    Q_INVOKABLE float sampleValue(double x);
    Q_INVOKABLE float sampleValueScaled(double x);

    int                    rowCount(const QModelIndex& parent) const final;
    QVariant               data(const QModelIndex& index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    double timestamp() const
    {
        return 0.0;
    }
    double duration() const
    {
        return 0.0;
    }
};

class MetricPhase : public QAbstractListModel
{
  public:
    enum MetricType
    {
        MetricValue,
        MetricSymbolic,
        MetricMarker,

        MetricImage,
    };

    Q_ENUM(MetricType)

  private:
    Q_OBJECT

    Q_PROPERTY(QString unit MEMBER m_unit)
    Q_PROPERTY(MetricType type MEMBER m_type)

    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QString category READ category)

    std::shared_ptr<MetricValues> m_template;

    std::map<quint64, std::shared_ptr<MetricValues>> m_values;
    std::vector<std::shared_ptr<MetricValues>>       m_list;

    quint64    m_id;
    QString    m_name;
    MetricType m_type;
    QString    m_unit;

  public:
    MetricPhase(QObject* parent = nullptr);

    friend class Metrics;

    enum FieldNames
    {
        FieldPhase,
        FieldIndex,
        FieldColor,
    };

    int                    rowCount(const QModelIndex& parent) const final;
    QVariant               data(const QModelIndex& index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    QString category() const
    {
        return "Metric";
    }

    Q_INVOKABLE QVariantList sampleValue(double x);
};

class Metrics : public QAbstractListModel
{
    Q_OBJECT

    std::map<quint64, std::shared_ptr<MetricPhase>> m_metric;
    std::vector<std::shared_ptr<MetricPhase>>       m_metricList;

    friend class MetricPhase;
    friend class MetricValues;
    friend class ScreenshotProvider;

    TraceMetaData& m_meta;

  public:
    Metrics(TraceMetaData& meta, QObject* parent = nullptr);

    enum FieldNames
    {
        FieldMetric,
        FieldMetricType,
        FieldName,
        FieldId,
        FieldNumPhases,
    };

    int                    rowCount(const QModelIndex& parent) const final;
    QVariant               data(const QModelIndex& index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    void insertValue(QJsonObject const& data, quint64 i);
    void populate(QJsonObject const& meta);
    void optimize();
  public slots:
    void updateView();
};
