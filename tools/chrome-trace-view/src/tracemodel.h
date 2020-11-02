#pragma once

#include <QAbstractListModel>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

#include <future>
#include <map>
#include <memory>

class QFile;

class TraceModel;
struct TraceMeta;

struct TraceProperties
{
    double      timestampBase;
    qint64      startTimestamp;
    TraceModel* model;
};

struct EventStats
{
    EventStats();

    quint64 num_events;
    double  average;
    double  max;
    double  min;
};

class EventModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(quint64 tid READ tid)
    Q_PROPERTY(quint64 pid READ pid)
    Q_PROPERTY(double timestamp READ timestamp)
    Q_PROPERTY(double duration READ duration)
    Q_PROPERTY(quint64 id READ id)

    Q_PROPERTY(double average READ average)
    Q_PROPERTY(double max READ max)
    Q_PROPERTY(double min READ min)
    Q_PROPERTY(quint64 numEvents READ numEvents)

    quint64     m_id;
    TraceMeta*  m_meta;
    QVariantMap m_eventData;
    TraceModel* m_trace;
    EventStats* m_stats;

  public:
    EventModel(TraceMeta* meta, EventStats* stats, TraceModel* trace);

    QString name() const;
    QString category() const;
    quint64 tid() const;
    quint64 pid() const;
    double  timestamp() const;
    double  duration() const;
    quint64 id() const;

    double  average() const;
    double  max() const;
    double  min() const;
    quint64 numEvents() const;
};

class ThreadModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(qint32 maxStack MEMBER m_maxStack)

  public:
    quint64 m_processId;
    quint64 m_threadId;
    QString m_name;
    qint32  m_maxStack;

    QJsonObject            m_trace;
    QJsonArray             m_events;
    std::vector<TraceMeta> m_eventMeta;
    std::vector<quint64>   m_visibleEvents;

    TraceProperties m_props;

    size_t m_viewStart;
    size_t m_viewEnd;

  public:
    std::map<quint64, EventStats> m_stats;
    size_t                        m_threadIdx;

    explicit ThreadModel(
        QJsonObject const&     trace,
        QJsonArray const&      events,
        quint64                pid,
        quint64                tid,
        QString const&         name,
        TraceProperties const& props,
        QObject*               parent = nullptr);

    enum EventType
    {
        EventBeginScope,
        EventEndScope,

        EventInstant,

        EventAsyncBegin,
        EventAsyncEnd,

        EventMeta,
    };

    Q_ENUM(EventType)

    enum EventField
    {
        FieldType,
        FieldName,
        FieldCategory,
        FieldPid,
        FieldTid,
        FieldTimestamp,
        FieldDuration,

        FieldStackDepth,
        FieldFillColor,
        FieldEventId,
    };

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

  signals:
    void parseUpdate(QString const& message);
};

class ProcessModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    quint64     m_processId;
    QString     m_name;
    QJsonObject m_trace;

    std::map<quint64, std::shared_ptr<ThreadModel>> m_threads;
    std::vector<std::shared_ptr<ThreadModel>>       m_threadList;

  public:
    ProcessModel(
        QJsonObject const&     trace,
        quint64                pid,
        QString const&         name,
        TraceProperties const& props,
        QObject*               parent = nullptr);

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    enum ThreadField
    {
        FieldName,
        FieldModel,
        FieldMaxStackDepth,
    };

  signals:
    void parseUpdate(QString const& message);
};

class NetDownload : public QObject
{
    Q_OBJECT

    QNetworkAccessManager m_man;

  public:
    NetDownload(QObject* parent = nullptr);

  public slots:
    void downloadFile(QString const& source);

  signals:
    void fileError(QString const& reason);
    void fileDownloaded(QByteArray const& data);
};

class ExtraInfoGroup : public QAbstractListModel
{
    Q_OBJECT

    std::vector<QString> m_keys;
    QJsonObject          m_data;
    QString              m_name;

  public:
    ExtraInfoGroup(
        QString const&     name,
        QJsonObject const& data,
        QObject*           parent = nullptr);

    QString name() const
    {
        return m_name;
    }

    enum FieldNames
    {
        FieldName,
        FieldValue
    };

    virtual int      rowCount(const QModelIndex& parent) const final;
    virtual QVariant data(const QModelIndex& index, int role) const final;
    virtual QHash<int, QByteArray> roleNames() const final;
};

class ExtraInfo : public QAbstractListModel
{
    Q_OBJECT

    TraceModel*                                  m_trace;
    std::vector<std::unique_ptr<ExtraInfoGroup>> m_groups;

  public:
    ExtraInfo(TraceModel* trace, QObject* parent = nullptr);

    enum FieldNames
    {
        FieldGroupName,
        FieldGroup
    };

    int                    rowCount(const QModelIndex& parent) const;
    QVariant               data(const QModelIndex& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

  public slots:
    void updateData();
};

class Metrics;
class ScreenshotProvider;

class MetricValues : public QAbstractListModel
{
    friend class ScreenshotProvider;

  public:
    enum MetricType
    {
        MetricValue,
        MetricSymbolic,
        MetricMarker,

        MetricImage,
    };

    Q_ENUM(MetricType)

    Q_PROPERTY(QString unit MEMBER m_unit)
    Q_PROPERTY(MetricType type MEMBER m_type)

    Q_PROPERTY(QString name MEMBER m_name)
    Q_PROPERTY(QString category READ category)

    Q_PROPERTY(double timestamp READ timestamp)
    Q_PROPERTY(double duration READ duration)

    Q_PROPERTY(double average MEMBER m_average)
    Q_PROPERTY(float max MEMBER m_maxValue)
    Q_PROPERTY(float min MEMBER m_minValue)
    Q_PROPERTY(quint64 numEvents MEMBER m_numEvents)

  private:
    Q_OBJECT

    friend class Metrics;

    struct Value
    {
        double  ts;
        float   value;
        quint64 i;
    };

    TraceModel*          m_trace;
    std::vector<Value>   m_values;
    std::vector<quint64> m_visible;

    QString    m_name;
    quint64    m_id;
    MetricType m_type;
    QString    m_unit;

    quint64 m_numEvents = 0;
    float   m_minValue  = std::numeric_limits<float>::max(),
          m_maxValue    = std::numeric_limits<float>::min();
    double m_average    = 0.f;
    double m_prevTime   = 0.f;

  public:
    MetricValues(Metrics* metrics, QObject* parent = nullptr);

    enum FieldNames
    {
        FieldValue,
        FieldIndex,
        FieldValueScaled,
        FieldPreviousValue,
        FieldPreviousValueScaled,
        FieldTimestamp,
        FieldPreviousTimestamp,
    };

    Q_INVOKABLE float sampleValue(double x);
    Q_INVOKABLE float sampleValueScaled(double x);

    int                    rowCount(const QModelIndex& parent) const final;
    QVariant               data(const QModelIndex& index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    QString category() const
    {
        return "Metric";
    }
    double timestamp() const
    {
        return 0.0;
    }
    double duration() const
    {
        return 0.0;
    }
};

class Metrics : public QAbstractListModel
{
    Q_OBJECT

    TraceModel*     m_trace;
    TraceProperties m_props;

    std::map<quint64, std::shared_ptr<MetricValues>> m_metric;
    std::vector<std::shared_ptr<MetricValues>>       m_metricList;

    friend class MetricValues;
    friend class ScreenshotProvider;

  public:
    Metrics(
        TraceModel*            trace,
        TraceProperties const& props,
        QObject*               parent = nullptr);

    enum FieldNames
    {
        FieldMetric,
        FieldMetricType,
        FieldName,
        FieldId,
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

class TraceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString source NOTIFY sourceChanged MEMBER m_source)

    Q_PROPERTY(double timestampBase READ timestampBase)
    Q_PROPERTY(
        double totalDuration READ totalDuration NOTIFY totalDurationChanged)

    Q_PROPERTY(double viewStart NOTIFY viewStartChanged MEMBER m_viewStart)
    Q_PROPERTY(double viewEnd NOTIFY viewEndChanged MEMBER m_viewEnd)
    Q_PROPERTY(double timePerPixel MEMBER m_timePerPixel)

    Q_PROPERTY(bool parsing MEMBER m_parsing)

    Q_PROPERTY(QObject* extraInfo READ extraInfo NOTIFY extraInfoChanged)

    Q_PROPERTY(QObject* metrics READ metrics NOTIFY metricsChanged)

    QString    m_source;
    ExtraInfo* m_extraInfo;

    std::unique_ptr<QFile>                           m_traceFile;
    QJsonObject                                      m_trace;
    const char*                                      m_traceSource;
    std::map<quint64, std::shared_ptr<ProcessModel>> m_processes;
    std::vector<std::shared_ptr<ProcessModel>>       m_processList;

    std::unique_ptr<Metrics> m_metrics;

    std::future<void> m_parseTask;

    double m_totalDuration;

    bool m_parsing;

  public:
    double m_viewStart, m_viewEnd, m_timePerPixel;

    explicit TraceModel(QObject* parent = nullptr);

    QJsonObject traceObject() const
    {
        return m_trace;
    }
    QJsonArray traceEvents() const
    {
        return m_trace.find("traceEvents").value().toArray();
    }

    void resetData();

    Q_INVOKABLE QObject* eventFromId(quint64 pid, quint64 tid, quint64 id);

    Q_INVOKABLE void openFile();

    Q_INVOKABLE void emscriptenAuto();

    double timestampBase() const;
    double totalDuration() const
    {
        return m_totalDuration;
    }
    QObject* extraInfo() const
    {
        return m_extraInfo;
    }
    QObject* metrics() const
    {
        return m_metrics.get();
    }

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void parseAccountingInfo(QByteArray const& profile, uchar* ptr = nullptr);

    const char* traceSource() const;

    enum ProcessField
    {
        FieldName,
        FieldModel,
        FieldPid,
    };

  signals:
    void sourceChanged(QString const& newSource);
    void viewStartChanged(double newViewStart);
    void viewEndChanged(double newViewEnd);
    void viewUpdated();

    void extraInfoChanged(QObject* extra);
    void metricsChanged(QObject* metrics);

    void totalDurationChanged(double totalDuration);

    void startingParse();
    void parseUpdate(QString const& message);
    void traceParsed();
    void traceReceived();
    void parseError(QString const& error);

  public slots:
};

struct TraceMeta
{
    quint64 index;
    quint64 stats_hash;
    qint64  duration;
    qint64  timestamp;
    qint32  stackDepth;
};
