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

struct TraceMeta;

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

  public:
    explicit ThreadModel(QJsonObject const& trace,
        QJsonArray const&  events,
        quint64            pid,
        quint64            tid,
        QString const&     name,
        qint64 baseTimestamp,
        QObject*           parent = nullptr);

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
    };

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;
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
    ProcessModel(QJsonObject const& trace,
        quint64            pid,
        QString const&     name,
        qint64 baseTimestamp,
        QObject*           parent = nullptr);

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    enum ThreadField
    {
        FieldName,
        FieldModel,
        FieldMaxStackDepth,
    };
};

class TraceModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString source NOTIFY sourceChanged MEMBER m_source)

    QString m_source;

    std::unique_ptr<QFile>                           m_traceFile;
    QJsonObject                                      m_trace;
    std::map<quint64, std::shared_ptr<ProcessModel>> m_processes;
    std::vector<std::shared_ptr<ProcessModel>>       m_processList;

    std::future<void> m_parseTask;

  public:
    explicit TraceModel(QObject* parent = nullptr);

    int                    rowCount(QModelIndex const& parent) const;
    QVariant               data(QModelIndex const& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void parseAccountingInfo();

    enum ProcessField
    {
        FieldName,
        FieldModel,
    };

  signals:
    void sourceChanged(QString const& newSource);

    void startingParse();
    void parseUpdate(QString const& message);
    void traceParsed();
    void parseError(QString const& error);

  public slots:
};

struct TraceMeta
{
    quint64 index;
    qint64 duration;
    qint64 timestamp;
    qint32 stackDepth;
};
