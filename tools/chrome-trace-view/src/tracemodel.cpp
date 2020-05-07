#include "tracemodel.h"

#include <QColor>
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <array>
#include <future>
#include <set>
#include <stack>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

EM_JS(char*, get_queries, (), {
    var lengthBytes = lengthBytesUTF8(window.location.search) + 1;
    var wasmString  = _malloc(lengthBytes);
    stringToUTF8(window.location.search, wasmString, lengthBytes + 1);
    return wasmString;
});

#endif

#include <QtDebug>

TraceModel::TraceModel(QObject* parent) :
    QAbstractListModel(parent), m_extraInfo(new ExtraInfo(this, this)),
    m_parsing(false)
{
    connect(this, &TraceModel::sourceChanged, [this](QString const& newSource) {
        resetData();

        m_traceFile = std::make_unique<QFile>(newSource);
        m_parsing   = true;

        emit beginInsertRows({}, 0, m_processes.size());

        auto parseWorker = [this]() {
            try
            {
                if(!m_traceFile->open(QFile::ReadOnly))
                {
                    emit parseError("File '" + m_source + "' not found");
                    return;
                }

                auto size = static_cast<quint32>(m_traceFile->size());
                auto ptr  = m_traceFile->map(0, size);

                QByteArray source;
                source.setRawData(reinterpret_cast<const char*>(ptr), size);

                parseAccountingInfo(source, ptr);
                m_parsing = false;
            } catch(std::exception const& e)
            {
                qWarning() << "Worker died with exception:" << e.what();
            }
        };

#if defined(__EMCSRIPTEN__)
        parseWorker();
#else
        m_parseTask = std::async(std::launch::async, parseWorker);
#endif
    });

    connect(this, &TraceModel::viewStartChanged, this, [this](double) {
        emit viewUpdated();
    });
    connect(this, &TraceModel::viewEndChanged, this, [this](double) {
        emit viewUpdated();
    });

    connect(
        this,
        &TraceModel::traceParsed,
        this,
        [this]() {
            emit endInsertRows();
            m_parsing = false;
        },
        Qt::QueuedConnection);
    connect(
        this,
        &TraceModel::parseError,
        this,
        [this](QString const&) { m_parsing = false; },
        Qt::QueuedConnection);
    connect(
        this, &TraceModel::traceReceived, m_extraInfo, &ExtraInfo::updateData);
}

void TraceModel::resetData()
{
    emit beginRemoveRows({}, 0, m_processes.size());

    m_processes.clear();
    m_processList.clear();
    m_trace         = {};
    m_traceFile     = {};
    m_totalDuration = 0.0;

    totalDurationChanged(0.0);

    emit endRemoveRows();
}

QObject* TraceModel::eventFromId(quint64 pid, quint64 tid, quint64 id)
{
    try
    {
        auto proc = m_processes.at(pid);

        auto thread = proc->m_threadList.at(tid);

        auto& meta = thread->m_eventMeta.at(id);

        return new EventModel(
            &meta, &thread->m_stats.at(meta.stats_hash), this);
    } catch(std::out_of_range const&)
    {
        return nullptr;
    }
}

void TraceModel::openFile()
{
    QFileDialog::getOpenFileContent(
        "Chrome Trace Files (*.json)",
        [this](QString const& filename, QByteArray const& data) {
            qDebug() << "Opening profile" << filename;

            resetData();

            emit beginInsertRows({}, 0, m_processes.size());

#if defined(__EMSCRIPTEN__)
            parseAccountingInfo(data);
#else
            m_parseTask = std::async(std::launch::async, [this, data]() {
                parseAccountingInfo(data);
            });
#endif
        });
}

void TraceModel::emscriptenAuto()
{
    QString source  = "";
    char*   queries = nullptr;

#if defined(__EMSCRIPTEN__)
    queries = get_queries();
#else
    queries = "?source=https://api.birchy.dev/v2/reports/482/json";
#endif

    if(!queries)
        qDebug() << "No query string";

    source = queries;

    if(!source.startsWith("?source="))
        return;

    source = source.split("&")[0].split("=")[1];

    NetDownload* downloader = new NetDownload(this);
    connect(
        downloader,
        &NetDownload::fileDownloaded,
        [this](QByteArray const& data) {
            qDebug() << "File downloaded";

            resetData();

            emit beginInsertRows({}, 0, m_processes.size());

            parseAccountingInfo(data);
        });

    downloader->downloadFile(source);
    qDebug() << "Downloading" << source;
}

double TraceModel::timestampBase() const
{
    if(auto timeUnitIt = m_trace.find("displayTimeUnit");
       timeUnitIt != m_trace.end())
    {
        auto timeUnit = timeUnitIt->toString();

        if(timeUnit == "us")
            return 1000000.0;
    }

    return 1000000.0;
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

void TraceModel::parseAccountingInfo(QByteArray const& profile, uchar* ptr)
{
    emit startingParse();

    QJsonParseError error;
    auto            trace = QJsonDocument::fromJson(profile, &error);

    if(m_traceFile)
    {
        m_traceFile->unmap(ptr);
        m_traceFile->close();
    }

    if(!trace.isObject())
    {
        if(error.error != QJsonParseError::NoError)
        {
            emit parseError(
                "Failed to find root object: " + error.errorString());
            return;
        }

        m_trace.insert("traceEvents", trace.array());
    } else
        m_trace = trace.object();

    qDebug() << m_trace.keys();
    emit traceReceived();

    auto eventIt = m_trace.find("traceEvents");

    if(eventIt == m_trace.end())
    {
        emit parseError("Failed to find 'traceEvents'");
        return;
    }

    TraceProperties traceProps;
    traceProps.model          = this;
    traceProps.startTimestamp = std::numeric_limits<qint64>::max();
    traceProps.timestampBase  = timestampBase();
    m_totalDuration           = 0;

    emit parseUpdate("Finding first timestamp...");
    for(auto event : eventIt->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        if(!ev.contains("ts"))
            continue;

        auto ts = ev["ts"].toString().toLongLong();

        traceProps.startTimestamp =
            std::min<qint64>(ts, traceProps.startTimestamp);
    }

    m_metrics = std::make_unique<Metrics>(this, traceProps);
    connect(
        this,
        &TraceModel::viewUpdated,
        m_metrics.get(),
        &Metrics::updateView,
        Qt::DirectConnection);

    emit parseUpdate("Finding duration...");
    for(auto event : eventIt->toArray())
    {
        auto ts = event.toObject().toVariantMap()["ts"].toString().toLongLong();

        m_totalDuration = std::max<double>(
            m_totalDuration,
            (ts - traceProps.startTimestamp) / traceProps.timestampBase);
    }
    emit totalDurationChanged(m_totalDuration);

    emit parseUpdate("Finding processes and metrics...");
    for(auto event : eventIt->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        /* Check for metrics */
        if(ev["ph"] == "M" && ev["name"] == "metric_name")
        {
            m_metrics->populate(event.toObject());
        }

        if(ev["ph"] != "M" || ev["name"] != "process_name")
            continue;

        auto pid = ev["pid"].toString().toULongLong();

        if(m_processes.find(pid) != m_processes.end())
            continue;

        m_processes.emplace(
            pid,
            std::make_unique<ProcessModel>(
                m_trace,
                pid,
                ev["args"].toMap()["name"].toString(),
                traceProps,
                this));
    }

    /* Insert metric events */
    for(auto event : eventIt->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        if(ev["ph"] == "m")
        {
            m_metrics->insertValue(event.toObject());
        }
    }

    m_metrics->optimize();

    emit metricsChanged(m_metrics.get());

    for(auto const& process : m_processes)
        m_processList.push_back(process.second);

    for(auto const& process : m_processes)
        for(auto const& thread : process.second->m_threads)
            qDebug() << process.second->m_name << thread.second->m_threadId
                     << thread.second->m_name;

    emit traceParsed();
}

ProcessModel::ProcessModel(
    const QJsonObject&     trace,
    quint64                pid,
    QString const&         name,
    TraceProperties const& props,
    QObject*               parent) :
    QAbstractListModel(),
    m_processId(pid), m_name(name), m_trace(trace)
{
    connect(this, &ProcessModel::parseUpdate, [parent](QString const& message) {
        qobject_cast<TraceModel*>(parent)->parseUpdate(message);
    });

    auto events = m_trace.find("traceEvents");

    if(events == m_trace.end())
        return;

    for(auto event : events->toArray())
    {
        auto ev = event.toObject().toVariantMap();

        if(ev["ph"] != "M" || ev["name"] != "thread_name")
            continue;

        bool tid_valid = false;
        auto tid       = ev["tid"].toString().toULongLong(&tid_valid);

        if(!tid_valid)
            continue;

        if(m_threads.find(tid) != m_threads.end())
            continue;

        m_threads.emplace(
            tid,
            std::make_unique<ThreadModel>(
                m_trace,
                events->toArray(),
                ev["pid"].toLongLong(),
                tid,
                ev["args"].toMap()["name"].toString(),
                props,
                this));
    }

    for(auto const& thread : m_threads)
    {
        thread.second->m_threadIdx = m_threadList.size();
        m_threadList.push_back(thread.second);
    }
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
        //        return 2;
        return m_threadList.at(index.row())->m_maxStack;
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
    QJsonObject const&     trace,
    QJsonArray const&      events,
    quint64                pid,
    quint64                tid,
    QString const&         name,
    TraceProperties const& props,
    QObject*               parent) :
    QAbstractListModel(),
    m_processId(pid), m_threadId(tid), m_name(name), m_maxStack(0),
    m_trace(trace), m_events(events), m_props(props), m_viewStart(0),
    m_viewEnd(0)
{
    connect(this, &ThreadModel::parseUpdate, [parent](QString const& message) {
        qobject_cast<ProcessModel*>(parent)->parseUpdate(message);
    });

    quint64 i = 0;

    std::stack<size_t> stack_frames;

    auto string_hash = std::hash<std::string>();

    emit parseUpdate("Gathering thread event metadata");
    for(auto event : m_events)
    {
        auto ev = event.toObject().toVariantMap();

        i++;

        if(ev.contains("pid") && ev["pid"] != m_processId)
            continue;
        if(ev.contains("tid") && ev["tid"] != m_threadId)
            continue;

        auto type = ev["ph"];

        if(type != "E" && ev.contains("ts"))
        {
            m_eventMeta.push_back({});

            auto& event      = m_eventMeta.back();
            event.index      = i - 1;
            event.stackDepth = stack_frames.size();
            event.timestamp =
                ev["ts"].toString().toLongLong() - props.startTimestamp;
        }

        if(type == "B")
        {
            stack_frames.push(m_eventMeta.size() - 1);
        } else if(type == "E")
        {
            auto& start_event = m_eventMeta.at(stack_frames.top());
            auto  start_event_data =
                m_events.at(start_event.index).toObject().toVariantMap();

            auto start_time = start_event.timestamp;
            auto end_time   = m_events.at(i - 1)
                                .toObject()
                                .toVariantMap()["ts"]
                                .toString()
                                .toLongLong() -
                            props.startTimestamp;

            start_event.duration = end_time - start_time;

            auto name = string_hash(ev["name"].toString().toStdString());
            start_event.stats_hash = name;

            auto stats_it = m_stats.find(name);

            if(stats_it == m_stats.end())
                stats_it = m_stats.emplace(name, EventStats()).first;

            auto& stats = stats_it->second;
            stats.max   = std::max<double>(stats.max, start_event.duration);
            stats.min   = std::min<double>(stats.min, start_event.duration);

            stats.num_events++;
            stats.average =
                stats.average +
                (start_event.duration - stats.average) / stats.num_events;

            stack_frames.pop();
        }

        m_maxStack = std::max<qint32>(m_maxStack, stack_frames.size());
    }

    /* Terminate missing stack frames */
    while(!stack_frames.empty())
    {
        auto& event = m_eventMeta.at(stack_frames.top());

        event.duration = (props.model->totalDuration() -
                          event.timestamp / props.model->timestampBase()) *
                         props.model->timestampBase();

        stack_frames.pop();
    }

    qDebug() << "Number of events:" << m_eventMeta.size() << "/"
             << m_events.size();

    connect(
        props.model,
        &TraceModel::viewUpdated,
        this,
        [this, model = props.model, timeBase = props.timestampBase]() {
            //            qDebug() << "Range" << model->m_viewStart <<
            //            model->m_viewEnd;

            std::vector<quint64> newEvents;
            size_t               i = 0;

            for(auto const& event : m_eventMeta)
            {
                auto eventStart = event.timestamp / timeBase;
                auto eventEnd =
                    event.timestamp / timeBase + event.duration / timeBase;

                i++;

                /* Ignore all events not visible from the start */
                /* Ignore events that are past the end of the view */
                if(eventEnd < model->m_viewStart ||
                   eventStart > model->m_viewEnd)
                    continue;

                auto visualDuration =
                    (event.duration / timeBase) * (1.0 / model->m_timePerPixel);
                if(visualDuration < 10)
                    continue;

                newEvents.push_back(i - 1);
            }

            std::vector<quint64> diff_set, added, remvd;

            std::set_symmetric_difference(
                m_visibleEvents.begin(),
                m_visibleEvents.end(),
                newEvents.begin(),
                newEvents.end(),
                std::back_inserter(diff_set));

            std::set_intersection(
                newEvents.begin(),
                newEvents.end(),
                diff_set.begin(),
                diff_set.end(),
                std::back_inserter(added));

            newEvents.clear();

            std::set_intersection(
                m_visibleEvents.begin(),
                m_visibleEvents.end(),
                diff_set.begin(),
                diff_set.end(),
                std::back_inserter(remvd));

            diff_set.clear();

            for(auto idx : remvd)
            {
                auto it = std::find(
                    m_visibleEvents.begin(), m_visibleEvents.end(), idx);
                beginRemoveRows(
                    {},
                    it - m_visibleEvents.begin(),
                    it - m_visibleEvents.begin());
                m_visibleEvents.erase(it);
                endRemoveRows();
            }

            for(auto idx : added)
            {
                auto it = std::find_if(
                    m_visibleEvents.begin(),
                    m_visibleEvents.end(),
                    [idx](quint64 otherIdx) { return otherIdx > idx; });

                beginInsertRows(
                    {},
                    it - m_visibleEvents.begin(),
                    it - m_visibleEvents.begin());
                m_visibleEvents.insert(it, idx);
                endInsertRows();
            }

            //            qDebug() << "Sort" << m_visibleEvents.size() <<
            //            added.size()
            //                     << remvd.size();
        },
        Qt::DirectConnection);
}

int ThreadModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_visibleEvents.size());
}

QVariant ThreadModel::data(const QModelIndex& index, int role) const
{
    auto visibleIndex = m_visibleEvents.at(index.row());

    auto eventIndex = m_eventMeta.at(visibleIndex).index;
    auto event      = m_events.at(eventIndex).toObject();

    const std::array<Qt::GlobalColor, 6> colors = {{
        Qt::blue,
        Qt::red,
        Qt::yellow,
        Qt::magenta,
        Qt::green,
        Qt::cyan,
    }};

    switch(role)
    {
    case FieldName:
        return event.find("name")->toVariant();
    case FieldCategory:
        return event.find("cat")->toVariant();
    case FieldPid:
        return event.find("pid")->toVariant();
    case FieldTid:
        return QVariant::fromValue(m_threadIdx);
    case FieldType:
        return event.find("ph")->toVariant();

    case FieldFillColor:
        return QColor(colors.at(std::rand() % colors.size())).darker(200);

    case FieldTimestamp:
        return m_eventMeta.at(visibleIndex).timestamp / m_props.timestampBase;
    case FieldDuration:
        return m_eventMeta.at(visibleIndex).duration / m_props.timestampBase;
    case FieldStackDepth:
        return m_eventMeta.at(visibleIndex).stackDepth;
    case FieldEventId:
        return visibleIndex;
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
            {FieldFillColor, "fillColor"},
            {FieldEventId, "eventId"}};
}

EventModel::EventModel(TraceMeta* meta, EventStats* stats, TraceModel* trace) :
    QObject(trace), m_id(meta->index), m_meta(meta), m_trace(trace),
    m_stats(stats)
{
    m_eventData = trace->traceObject()
                      .find("traceEvents")
                      ->toArray()
                      .at(meta->index)
                      .toObject()
                      .toVariantMap();
}

QString EventModel::name() const
{
    return m_eventData["name"].toString();
}

QString EventModel::category() const
{
    return m_eventData["cat"].toString();
}

quint64 EventModel::tid() const
{
    return m_eventData["tid"].toString().toULongLong();
}

quint64 EventModel::pid() const
{
    return m_eventData["pid"].toString().toULongLong();
}

double EventModel::timestamp() const
{
    return m_meta->timestamp / m_trace->timestampBase();
}

double EventModel::duration() const
{
    return m_meta->duration / m_trace->timestampBase();
}

quint64 EventModel::id() const
{
    return m_id;
}

double EventModel::average() const
{
    return m_stats->average / m_trace->timestampBase();
}

double EventModel::max() const
{
    return m_stats->max / m_trace->timestampBase();
}

double EventModel::min() const
{
    return m_stats->min / m_trace->timestampBase();
}

quint64 EventModel::numEvents() const
{
    return m_stats->num_events;
}

EventStats::EventStats() :
    num_events(0), average(0), max(std::numeric_limits<float>::min()),
    min(std::numeric_limits<float>::max())
{
}

NetDownload::NetDownload(QObject* parent) : QObject(parent)
{
    connect(
        &m_man, &QNetworkAccessManager::finished, [this](QNetworkReply* reply) {
            emit fileDownloaded(reply->readAll());
            reply->deleteLater();
        });
}

void NetDownload::downloadFile(QString const& source)
{
    QNetworkRequest req;
    req.setUrl(source);

    auto reply = m_man.get(req);
    connect(
        reply,
        static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(
            &QNetworkReply::error),
        [this, reply](QNetworkReply::NetworkError) {
            emit fileError(reply->errorString());
        });
}

ExtraInfo::ExtraInfo(TraceModel* trace, QObject* parent) :
    QAbstractListModel(parent), m_trace(trace)
{
}

int ExtraInfo::rowCount(const QModelIndex& parent) const
{
    return m_groups.size();
}

QVariant ExtraInfo::data(const QModelIndex& index, int role) const
{
    if(index.row() >= m_groups.size())
        return {};

    auto const& val = m_groups.at(index.row());

    switch(role)
    {
    case FieldGroupName:
        return val->name();
    case FieldGroup:
        return QVariant::fromValue(val.get());
    default:
        return {};
    }
}

QHash<int, QByteArray> ExtraInfo::roleNames() const
{
    return {{FieldGroup, "group"}, {FieldGroupName, "groupName"}};
}

static bool isProfileInfo(QString const& key)
{
    static const std::array<const char*, 6> included_keys = {
        {"application", "build", "runtime", "device", "processor", "extra"}};

    for(auto const& includedKey : included_keys)
        if(includedKey == key.toStdString())
            return true;
    return false;
}

void ExtraInfo::updateData()
{
    emit beginRemoveRows({}, 0, m_groups.size());
    m_groups.clear();
    emit endRemoveRows();

    auto trace = m_trace->traceObject();

    std::vector<QString> selectedKeys;

    for(auto val : trace.keys())
        if(isProfileInfo(val))
            selectedKeys.push_back(val);

    emit beginInsertRows({}, 0, selectedKeys.size());
    for(auto val : trace.keys())
    {
        if(!isProfileInfo(val))
            continue;

        m_groups.push_back(
            std::make_unique<ExtraInfoGroup>(val, trace.find(val)->toObject()));
    }

    emit endInsertRows();

    emit m_trace->extraInfoChanged(this);
}

ExtraInfoGroup::ExtraInfoGroup(
    const QString& name, QJsonObject const& data, QObject* parent) :
    QAbstractListModel(parent),
    m_data(data), m_name(name)
{
    for(auto const& key : data.keys())
        m_keys.push_back(key);

    std::sort(m_keys.begin(), m_keys.end());
}

int ExtraInfoGroup::rowCount(const QModelIndex& parent) const
{
    return m_keys.size();
}

QVariant ExtraInfoGroup::data(const QModelIndex& index, int role) const
{
    if(index.row() >= m_keys.size())
        return {};

    auto row = m_data.find(m_keys.at(index.row()));

    switch(role)
    {
    case FieldName:
        return row.key();
    case FieldValue:
        return row.value();
    default:
        return {};
    }
}

QHash<int, QByteArray> ExtraInfoGroup::roleNames() const
{
    return {{FieldName, "infoKey"}, {FieldValue, "infoValue"}};
}

Metrics::Metrics(
    TraceModel* trace, TraceProperties const& props, QObject* parent) :
    QAbstractListModel(parent),
    m_trace(trace), m_props(props)
{
}

int Metrics::rowCount(const QModelIndex& parent) const
{
    return m_metric.size();
}

QVariant Metrics::data(const QModelIndex& index, int role) const
{
    if(index.row() >= m_metricList.size())
        return {};

    switch(role)
    {
    case FieldMetric:
        return QVariant::fromValue(m_metricList.at(index.row()).get());
    case FieldMetricType:
        return m_metricList.at(index.row())->m_type;
    case FieldName:
        return m_metricList.at(index.row())->m_name;
    case FieldId:
        return m_metricList.at(index.row())->m_id;
    default:
        return {};
    }
}

QHash<int, QByteArray> Metrics::roleNames() const
{
    return {
        {FieldMetric, "metric"},
        {FieldMetricType, "metricType"},
        {FieldName, "metricName"},
        {FieldId, "metricId"},
    };
}

void Metrics::insertValue(const QJsonObject& data)
{
    auto id_it    = data.find("id");
    auto value_it = data.find("v");
    auto ts_it    = data.find("ts");

    if(id_it == data.end() || value_it == data.end() || ts_it == data.end())
        return;

    auto id = (*id_it).toInt();

    auto metric_it = m_metric.find(id);

    if(metric_it == m_metric.end())
        return;

    auto metric = metric_it->second;

    auto ts =
        (ts_it->toVariant().toString().toULongLong() - m_props.startTimestamp) /
        m_props.timestampBase;

    metric->m_values.push_back({ts, value_it->toString().toFloat()});

    auto& newValue     = metric->m_values.back();
    metric->m_minValue = std::min(metric->m_minValue, newValue.value);
    metric->m_maxValue = std::max(metric->m_maxValue, newValue.value);
}

void Metrics::populate(const QJsonObject& meta)
{
    auto args_it = meta.find("args");
    auto id_it   = meta.find("id");

    if(args_it == meta.end() || id_it == meta.end())
        return;

    auto args = args_it->toObject().toVariantHash();
    auto id   = id_it->toInt();

    auto metric = std::make_shared<MetricValues>(this, this);

    metric->m_name = args["name"].toString();
    metric->m_id   = id;

    switch(args["type"].toInt())
    {
    case 1:
        metric->m_type = MetricValues::MetricSymbolic;
        break;
    case 2:
        metric->m_type = MetricValues::MetricMarker;
        break;
    default:
        metric->m_type = MetricValues::MetricValue;
        break;
    }

    m_metric.insert({id, metric});
    m_metricList.push_back(metric);
}

void Metrics::optimize()
{
    for(auto& metric : m_metric)
    {
        auto& values = *metric.second;

        if(values.m_maxValue == values.m_minValue)
        {
            auto end_ts = values.m_values.back().ts;

            values.m_values.clear();

            values.m_values.push_back({0, values.m_maxValue});
            values.m_values.push_back({end_ts, values.m_minValue});
            continue;
        }

        if(values.m_values.size() < 1)
            continue;

        float               previous = values.m_values.front().value;
        std::vector<size_t> to_remove;

        for(size_t i = 0; i < values.m_values.size(); i++)
        {
            auto current = values.m_values.at(i).value;
            if(previous == current)
                to_remove.push_back(i);
        }

        for(auto it = to_remove.rbegin(); it != to_remove.rend(); ++it)
        {
            if(*it == (values.m_values.size() - 1) && *it != 0)
                continue;

            values.m_values.erase(values.m_values.begin() + *it);
        }
    }
}

void Metrics::updateView()
{
    auto start = m_trace->m_viewStart, end = m_trace->m_viewEnd;

    for(auto const& metric_ : m_metricList)
    {
        auto& metric = *metric_;
        auto& values = metric.m_values;

        std::vector<quint64> visible;

        quint64 i = 0;
        for(auto const& v : values)
        {
            auto ev_start = i > 0 ? values[i - 1].ts : 0.0, ev_end = v.ts;

            i++;

            if(ev_end < start || ev_start > end)
                continue;

            visible.push_back(i - 1);
        }

        std::vector<quint64> diff, add, rem;

        std::set_symmetric_difference(
            metric.m_visible.begin(),
            metric.m_visible.end(),
            visible.begin(),
            visible.end(),
            std::back_inserter(diff));

        std::set_intersection(
            visible.begin(),
            visible.end(),
            diff.begin(),
            diff.end(),
            std::back_inserter(add));

        std::set_intersection(
            metric.m_visible.begin(),
            metric.m_visible.end(),
            diff.begin(),
            diff.end(),
            std::back_inserter(rem));

        for(auto const& i : rem)
        {
            auto it =
                std::find(metric.m_visible.begin(), metric.m_visible.end(), i);
            metric.beginRemoveRows(
                {},
                it - metric.m_visible.begin(),
                it - metric.m_visible.begin());
            metric.m_visible.erase(it);
            metric.endRemoveRows();
        }

        for(auto const& i : add)
        {
            auto it = std::find_if(
                metric.m_visible.begin(),
                metric.m_visible.end(),
                [i](quint64 other) { return other > i; });
            metric.beginInsertRows(
                {},
                it - metric.m_visible.begin(),
                it - metric.m_visible.begin());
            metric.m_visible.insert(it, i);
            metric.endInsertRows();
        }
    }
}

MetricValues::MetricValues(Metrics* metrics, QObject* parent) :
    QAbstractListModel(parent), m_trace(metrics->m_trace)
{
}

static float metric_value_scale(float v, float min_val, float max_val)
{
    auto range = max_val - min_val;

    if(range < 0.05f)
        return 1.f;

    return (v - min_val) / range;
}

float MetricValues::sampleValue(double x)
{
    /* TODO: Do a binary search for speed */

    for(auto const& v : m_values)
        if(v.ts >= x)
            return v.value;
    return 1.f;
}

float MetricValues::sampleValueScaled(double x)
{
    return metric_value_scale(sampleValue(x), m_minValue, m_maxValue);
}

int MetricValues::rowCount(const QModelIndex& parent) const
{
    return m_visible.size();
}

QVariant MetricValues::data(const QModelIndex& index, int role) const
{
    if(index.row() >= m_visible.size())
        return {};

    auto idx = m_visible.at(index.row());

    switch(role)
    {
    case FieldValue:
        return m_values.at(idx).value;
    case FieldValueScaled:
        return metric_value_scale(
            m_values.at(idx).value, m_minValue, m_maxValue);

    case FieldTimestamp:
        return m_values.at(idx).ts;

    case FieldPreviousValue:
        return idx < 1 ? 0.f : m_values.at(index.row() - 1).value;
    case FieldPreviousValueScaled:
        return idx < 1
                   ? 0.f
                   : metric_value_scale(
                         m_values.at(idx - 1).value, m_minValue, m_maxValue);

    case FieldPreviousTimestamp:
        return idx < 1 ? 0 : m_values.at(idx - 1).ts;
    default:
        return {};
    }
}

QHash<int, QByteArray> MetricValues::roleNames() const
{
    return {
        {FieldValue, "value"},
        {FieldValueScaled, "valueScaled"},
        {FieldTimestamp, "ts"},
        {FieldPreviousValue, "previousValue"},
        {FieldPreviousValueScaled, "previousValueScaled"},
        {FieldPreviousTimestamp, "previousTs"},
    };
}
