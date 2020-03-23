#pragma once

#include <QAbstractListModel>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

#include <future>
#include <map>
#include <memory>

class QFile;

struct TraceModel;
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

    QString m_source;

    std::unique_ptr<QFile>                           m_traceFile;
    QJsonObject                                      m_trace;
    std::map<quint64, std::shared_ptr<ProcessModel>> m_processes;
    std::vector<std::shared_ptr<ProcessModel>>       m_processList;

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

    Q_INVOKABLE QObject* eventFromId(quint64 pid, quint64 tid, quint64 id);

    double timestampBase() const;
    double totalDuration() const
    {
        return m_totalDuration;
    }

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void parseAccountingInfo();

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

    void totalDurationChanged(double totalDuration);

    void startingParse();
    void parseUpdate(QString const& message);
    void traceParsed();
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
