#include "tracemodel.h"

#include <QColor>
#include <QFile>
#include <QJsonDocument>
#include <future>
#include <stack>

#include <QtDebug>

TraceModel::TraceModel(QObject* parent) : QAbstractListModel(parent)
{
    qDebug("Hello");

    connect(this, &TraceModel::sourceChanged, [this](QString const& newSource) {
        m_traceFile = std::make_unique<QFile>(newSource);

        emit beginRemoveRows({}, 0, m_processes.size());
        emit endRemoveRows();

        m_parseTask =
            std::async(std::launch::async, [this]() { parseAccountingInfo(); });
    });

    QObject::connect(
        this,
        &TraceModel::traceParsed,
        this,
        [this]() {
            emit beginInsertRows({}, 0, m_processes.size());
            emit endInsertRows();
        },
        Qt::QueuedConnection);
}

int TraceModel::rowCount(QModelIndex const&) const
{
    return static_cast<int>(m_processes.size());
}

QVariant TraceModel::data(QModelIndex const& index, int role) const
{
    switch(role)
    {
    case FieldName:
        return m_processList.at(index.row())->m_name;
    case FieldModel:
        return QVariant::fromValue(m_processList.at(index.row()).get());
    default:
        return {};
    }
}

QHash<int, QByteArray> TraceModel::roleNames() const
{
    return {{FieldName, "name"}, {FieldModel, "threads"}};
}

void TraceModel::parseAccountingInfo()
{
    emit startingParse();

    if(!m_traceFile->open(QFile::ReadOnly))
    {
        emit parseError("File '" + m_source + "' not found");
        return;
    }

    auto size = static_cast<quint32>(m_traceFile->size());
    auto ptr  = m_traceFile->map(0, size);

    QByteArray      source;
    QJsonParseError error;
    source.setRawData(reinterpret_cast<const char*>(ptr), size);
    auto trace = QJsonDocument::fromJson(source, &error);

    if(!trace.isObject())
    {
        emit parseError("Failed to find root object: " + error.errorString());
        return;
    }

    m_trace = trace.object();

    auto eventIt = m_trace.find("traceEvents");

    if(eventIt == m_trace.end())
    {
        emit parseError("Failed to find 'traceEvents'");
        return;
    }

    emit parseUpdate("Finding processes...");
    for(auto event : eventIt->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        if(ev["ph"] != "M" && ev["name"] != "process_name")
            continue;

        auto pid = ev["pid"].toString().toULongLong();

        if(m_processes.find(pid) != m_processes.end())
            continue;

        m_processes.emplace(
            pid,
            std::make_unique<ProcessModel>(
                m_trace, pid, ev["args"].toMap()["name"].toString()));
    }

    for(auto const& process : m_processes)
        m_processList.push_back(process.second);

    for(auto const& process : m_processes)
        for(auto const& thread : process.second->m_threads)
            qDebug() << process.second->m_name << thread.second->m_threadId
                     << thread.second->m_name;

    emit traceParsed();
}

ProcessModel::ProcessModel(
    const QJsonObject& trace,
    quint64            pid,
    QString const&     name,
    QObject*           parent) :
    QAbstractListModel(parent),
    m_processId(pid), m_name(name), m_trace(trace)
{
    qDebug() << "ProcessModel";

    auto events = m_trace.find("traceEvents");

    if(events == m_trace.end())
        return;

    for(auto event : events->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        if(ev["ph"] != "M" && ev["name"] != "thread_name")
            continue;

        auto tid = ev["tid"].toString().toULongLong();

        if(m_threads.find(tid) != m_threads.end())
            continue;

        m_threads.emplace(
            tid,
            std::make_unique<ThreadModel>(
                m_trace,
                events->toArray(),
                ev["pid"].toLongLong(),
                tid,
                ev["args"].toMap()["name"].toString()));
    }

    for(auto const& thread : m_threads)
        m_threadList.push_back(thread.second);
}

int ProcessModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_threads.size());
}

QVariant ProcessModel::data(const QModelIndex& index, int role) const
{
    switch(role)
    {
    case FieldName:
        return m_threadList.at(index.row())->m_name;
    case FieldModel:
        return QVariant::fromValue(m_threadList.at(index.row()).get());
    case FieldMaxStackDepth:
        return 2;
        //        return m_threadList.at(index.row())->m_maxStack;
    default:
        return {};
    }
}

QHash<int, QByteArray> ProcessModel::roleNames() const
{
    return {{FieldName, "name"},
            {FieldModel, "events"},
            {FieldMaxStackDepth, "maxDepth"}};
}

ThreadModel::ThreadModel(
    QJsonObject const& trace,
    QJsonArray const&  events,
    quint64            pid,
    quint64            tid,
    const QString&     name,
    QObject*           parent) :
    QAbstractListModel(parent),
    m_processId(pid), m_threadId(tid), m_name(name), m_trace(trace),
    m_events(events)
{
    qDebug() << "ThreadModel";

    qint32  stackDepth = 0;
    quint64 i          = 0;

    std::stack<size_t> stack_frames;

    for(auto event : m_events)
    {
        auto ev = event.toObject().toVariantMap();

        if(ev.contains("pid") && ev["pid"] != m_processId)
            continue;
        if(ev.contains("tid") && ev["tid"] != m_threadId)
            continue;

        auto type = ev["ph"];

        if(type != "E")
        {
            m_eventMeta.push_back({});

            auto& event      = m_eventMeta.back();
            event.index      = i;
            event.stackDepth = stackDepth;
        }

        if(type == "B")
        {
            stack_frames.push(m_eventMeta.size() - 1);
            stackDepth++;
        } else if(type == "E")
        {
            auto start_event_data =
                m_events.at(stack_frames.top()).toObject().toVariantMap();

            auto start_time = ev["ts"].toString().toULongLong();
            auto end_time   = start_event_data["ts"].toString().toULongLong();

            auto& start_event    = m_eventMeta.at(stack_frames.top());
            start_event.duration = end_time - start_time;

            stack_frames.pop();
            stackDepth--;
        }

        m_maxStack = std::max(m_maxStack, stackDepth);
    }
}

int ThreadModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_eventMeta.size());
}

QVariant ThreadModel::data(const QModelIndex& index, int role) const
{
    auto eventIndex = m_eventMeta.at(index.row()).index;
    auto event      = m_events.at(eventIndex).toObject();

    switch(role)
    {
    case FieldName:
        return event.find("name")->toVariant();
    case FieldTimestamp:
        return event.find("ts")->toVariant();
    case FieldCategory:
        return event.find("cat")->toVariant();
    case FieldPid:
        return event.find("pid")->toVariant();
    case FieldTid:
        return event.find("tid")->toVariant();
    case FieldType:
        return event.find("ph")->toVariant();

    case FieldFillColor:
        return QColor(Qt::red);

    case FieldDuration:
        return m_eventMeta.at(index.row()).duration;
    case FieldStackDepth:
        return m_eventMeta.at(index.row()).stackDepth;
    default:
        return {};
    }
}

QHash<int, QByteArray> ThreadModel::roleNames() const
{
    return {{FieldType, "type"},
            {FieldName, "name"},
            {FieldCategory, "category"},
            {FieldPid, "pid"},
            {FieldTid, "tid"},
            {FieldTimestamp, "ts"},
            {FieldDuration, "time"},
            {FieldStackDepth, "stackDepth"},
            {FieldFillColor, "fillColor"}};
}
