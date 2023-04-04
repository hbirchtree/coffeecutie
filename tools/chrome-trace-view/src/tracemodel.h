#pragma once

#include <QAbstractListModel>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSortFilterProxyModel>

#include <future>
#include <map>
#include <memory>

#include "extrainfo.h"
#include "metrics.h"

#include "shared_fields.h"

class QFile;

class TraceModel;
struct EventMetaData;

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

    quint64        m_id;
    EventMetaData* m_meta;
    QVariantMap    m_eventData;
    TraceModel*    m_trace;
    EventStats*    m_stats;
    double         m_timestampDivisor;

  public:
    EventModel(
        EventMetaData* meta,
        EventStats*    stats,
        TraceMetaData& traceMeta,
        QObject*       parent);

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

    TraceMetaData&                m_meta;
    std::vector<EventMetaData>    m_eventMeta;
    std::map<quint64, EventStats> m_stats;

  public:
    size_t m_threadIdx;

    explicit ThreadModel(
        TraceMetaData& metadata,
        quint64        pid,
        quint64        tid,
        QString const& name,
        QObject*       parent = nullptr);

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

        FieldStackDepth,
        FieldFillColor,
        FieldEventId,

        FieldTimestamp = TIMESTAMP_ROLE,
        FieldDuration  = DURATION_ROLE,
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
    quint64       m_processId;
    QString       m_name;
    TraceMetaData m_meta;

    std::map<quint64, std::shared_ptr<ThreadModel>> m_threads;
    std::vector<std::shared_ptr<ThreadModel>>       m_threadList;

  public:
    ProcessModel(
        TraceMetaData& meta,
        quint64        pid,
        QString const& name,
        QObject*       parent = nullptr);

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

class MetaData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString graphicsApi READ graphicsApi)

    ExtraInfo* m_extra;

  public:
    MetaData(ExtraInfo* extra, QObject* parent = nullptr) :
        QObject(parent), m_extra(extra)
    {
    }

    QString graphicsApi() const;
};

class TraceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString source NOTIFY sourceChanged MEMBER m_source)

    Q_PROPERTY(bool parsing MEMBER m_parsing)

    Q_PROPERTY(QObject* extraInfo READ extraInfo NOTIFY extraInfoChanged)
    Q_PROPERTY(QObject* metrics READ metrics NOTIFY metricsChanged)
    Q_PROPERTY(QObject* meta READ meta NOTIFY metaChanged)

    std::unique_ptr<QFile>       m_traceFile;
    std::shared_ptr<QJsonObject> m_trace;
    std::shared_ptr<QJsonArray>  m_events;
    TraceMetaData                m_traceMeta;
    const char*                  m_traceSource;

    std::map<quint64, std::shared_ptr<ProcessModel>> m_processes;
    std::vector<std::shared_ptr<ProcessModel>>       m_processList;

    QString m_source;

    std::unique_ptr<ExtraInfo> m_extraInfo;
    std::unique_ptr<Metrics>   m_metrics;
    std::unique_ptr<MetaData>  m_meta;

    std::future<void> m_parseTask;

    bool m_parsing;

  public:
    explicit TraceModel(QObject* parent = nullptr);

    QJsonObject traceObject() const
    {
        return *m_trace;
    }
    QJsonArray traceEvents() const
    {
        return *m_events;
    }

    void resetData();

    Q_INVOKABLE QObject* eventFromId(quint64 pid, quint64 tid, quint64 id);

    Q_INVOKABLE void openFile();

    Q_INVOKABLE void emscriptenAuto();
    Q_INVOKABLE void filesystemAuto();

    QObject* extraInfo() const
    {
        return m_extraInfo.get();
    }
    QObject* meta() const
    {
        return m_meta.get();
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

  protected slots:
    void startParse(QByteArray const& data);

  signals:
    void sourceChanged(QString const& newSource);
    void viewStartChanged(double newViewStart);
    void viewEndChanged(double newViewEnd);
    void viewUpdated();

    void extraInfoChanged(QObject* extra);
    void metricsChanged(QObject* metrics);
    void metaChanged(QObject* meta);

    void totalDurationChanged(double totalDuration);

    void startingParse();
    void parseUpdate(QString const& message);
    void traceParsed();
    void traceReceived(QJsonObject const& trace);
    void parseError(QString const& error);
};

struct EventMetaData
{
    quint64 index;
    quint64 stats_hash;
    qint64  duration;
    qint64  timestamp;
    qint32  stackDepth;
};
