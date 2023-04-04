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
    QAbstractListModel(parent),
    m_extraInfo(std::make_unique<ExtraInfo>(this, this)),
    m_meta(std::make_unique<MetaData>(m_extraInfo.get(), this)),
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
        this,
        &TraceModel::traceReceived,
        m_extraInfo.get(),
        &ExtraInfo::updateData);
}

void TraceModel::resetData()
{
    emit beginRemoveRows({}, 0, m_processes.size());

    m_processes.clear();
    m_processList.clear();
    m_trace     = {};
    m_traceFile = {};

    emit endRemoveRows();
}

QObject* TraceModel::eventFromId(quint64 pid, quint64 tid, quint64 id)
{
    auto event = m_events->at(id);

    return nullptr;
}

void TraceModel::openFile()
{
    QFileDialog::getOpenFileContent(
        "Chrome Trace Files (*.json)",
        [this](QString const& filename, QByteArray const& data) {
            qDebug() << "Opening profile" << filename;

            startParse(data);
        });
}

void TraceModel::emscriptenAuto()
{
    QString source  = "";
    char*   queries = nullptr;

#if defined(__EMSCRIPTEN__)
    queries = get_queries();
#else
    queries =
        const_cast<char*>("?source=https://api.birchy.dev/v2/reports/482/json");
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
        this,
        [this](QByteArray const& data) {
            qDebug() << "File downloaded";

            startParse(data);
        });
    auto destroy = [downloader] { downloader->deleteLater(); };
    connect(downloader, &NetDownload::fileDownloaded, downloader, destroy);
    connect(downloader, &NetDownload::fileError, downloader, destroy);

    downloader->downloadFile(source);
    qDebug() << "Downloading" << source;
}

void TraceModel::filesystemAuto()
{
    qDebug() << qApp->arguments();
    if (qApp->arguments().size() != 2)
        return;
    QFile data(qApp->arguments().at(1));

    if (!data.open(QIODevice::ReadOnly))
        return;

    auto dataPtr = data.map(0, data.size());

    startParse(
        QByteArray(reinterpret_cast<const char*>(dataPtr), data.size()));
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

    } else
        m_trace = std::make_shared<QJsonObject>(trace.object());

    m_traceMeta.trace = m_trace;
    {
        auto timeUnit = m_trace->find("displayTimeUnit")->toString();
        if (timeUnit == "ns")
            m_traceMeta.timestampDivisor = 1'000'000'000'000.0;
        else if(timeUnit == "us")
            m_traceMeta.timestampDivisor = 1'000'000'000.0;
        else
            m_traceMeta.timestampDivisor = 1'000'000.0;
    }
    emit traceReceived(*m_trace);

    auto eventIt = m_trace->find("traceEvents");
    if(eventIt == m_trace->end())
    {
        emit parseError("Failed to find 'traceEvents'");
        return;
    }
    m_events           = std::make_shared<QJsonArray>(eventIt->toArray());
    m_traceMeta.events = m_events;

    //    TraceProperties traceProps;
    //    traceProps.model          = this;
    //    traceProps.startTimestamp = std::numeric_limits<qint64>::max();
    //    traceProps.timestampBase  = timestampBase();
    //    m_totalDuration           = 0;

    emit parseUpdate("Finding first timestamp...");
    for(auto event : *m_events)
    {
        auto ev = event.toObject().toVariantMap();

        if(!ev.contains("ts"))
            continue;

        auto ts = ev["ts"].toString().toLongLong();

        m_traceMeta.firstTimestamp =
            std::min<qint64>(ts, m_traceMeta.firstTimestamp);
    }
    emit parseUpdate("Finding duration...");
    for(auto event : *m_events)
    {
        auto ts = event.toObject().toVariantMap()["ts"].toString().toLongLong();

        m_traceMeta.totalDuration = std::max<qint64>(
            m_traceMeta.totalDuration, ts - m_traceMeta.firstTimestamp);
    }

    emit parseUpdate("Finding processes and metrics...");
    m_metrics = std::make_unique<Metrics>(m_traceMeta, this);
    for(auto event : *m_events)
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
                m_traceMeta, pid, ev["args"].toMap()["name"].toString(), this));
    }

    /* Insert metric events */
    quint64 i = 0;
    for(auto event : *m_events)
    {
        auto ev = event.toObject().toVariantMap();

        if(ev["ph"] == "m")
        {
            m_metrics->insertValue(event.toObject(), i);
        }
        i++;
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

const char* TraceModel::traceSource() const
{
    return m_traceSource;
}

void TraceModel::startParse(const QByteArray &data)
{
    resetData();

    beginInsertRows({}, 0, m_processes.size());

#if defined(__EMSCRIPTEN__)
    parseAccountingInfo(data);
#else
    m_parseTask = std::async(std::launch::async, [this, data]() {
        parseAccountingInfo(data);
    });
#endif
}

ProcessModel::ProcessModel(
    TraceMetaData& meta, quint64 pid, QString const& name, QObject* parent) :
    QAbstractListModel(),
    m_processId(pid), m_name(name), m_meta(meta)
{
    connect(this, &ProcessModel::parseUpdate, [parent](QString const& message) {
        qobject_cast<TraceModel*>(parent)->parseUpdate(message);
    });

    for(auto event : *m_meta.events)
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
                m_meta,
                ev["pid"].toLongLong(),
                tid,
                ev["args"].toMap()["name"].toString(),
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
    return {
        {FieldName, "name"},
        {FieldModel, "events"},
        {FieldMaxStackDepth, "maxDepth"}};
}

ThreadModel::ThreadModel(
    TraceMetaData& metadata,
    quint64        pid,
    quint64        tid,
    QString const& name,
    QObject*       parent) :
    QAbstractListModel(),
    m_processId(pid), m_threadId(tid), m_name(name), m_maxStack(0),
    m_meta(metadata)
{
    connect(this, &ThreadModel::parseUpdate, [parent](QString const& message) {
        qobject_cast<ProcessModel*>(parent)->parseUpdate(message);
    });

    quint64 i = 0;

    std::stack<size_t> stack_frames;

    auto string_hash = std::hash<std::string>();

    emit parseUpdate("Gathering thread event metadata");
    for(auto event : *m_meta.events)
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
                ev["ts"].toString().toLongLong() - m_meta.firstTimestamp;
        }

        if(type == "B")
        {
            stack_frames.push(m_eventMeta.size() - 1);
        } else if(type == "E")
        {
            auto& start_event = m_eventMeta.at(stack_frames.top());
            auto  start_event_data =
                m_meta.events->at(start_event.index).toObject().toVariantMap();

            auto start_time = start_event.timestamp;
            auto end_time   = m_meta.events->at(i - 1)
                                .toObject()
                                .toVariantMap()["ts"]
                                .toString()
                                .toLongLong() -
                            m_meta.firstTimestamp;

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

        event.duration = m_meta.totalDuration - event.timestamp;

        stack_frames.pop();
    }

    qDebug() << "Number of events:" << m_eventMeta.size() << "/"
             << m_meta.events->size();

//    connect(
//        props.model,
//        &TraceModel::viewUpdated,
//        this,
//        [this, model = props.model, timeBase = props.timestampBase]() {
//            //            qDebug() << "Range" << model->m_viewStart <<
//            //            model->m_viewEnd;

//            std::vector<quint64> newEvents;
//            size_t               i = 0;

//            for(auto const& event : m_eventMeta)
//            {
//                auto eventStart = event.timestamp / timeBase;
//                auto eventEnd =
//                    event.timestamp / timeBase + event.duration / timeBase;

//                i++;

//                /* Ignore all events not visible from the start */
//                /* Ignore events that are past the end of the view */
//                if(eventEnd < model->m_viewStart ||
//                   eventStart > model->m_viewEnd)
//                    continue;

//                auto visualDuration =
//                    (event.duration / timeBase) * (1.0 / model->m_timePerPixel);
//                if(visualDuration < 10)
//                    continue;

//                newEvents.push_back(i - 1);
//            }

//            std::vector<quint64> diff_set, added, remvd;

//            std::set_symmetric_difference(
//                m_visibleEvents.begin(),
//                m_visibleEvents.end(),
//                newEvents.begin(),
//                newEvents.end(),
//                std::back_inserter(diff_set));

//            std::set_intersection(
//                newEvents.begin(),
//                newEvents.end(),
//                diff_set.begin(),
//                diff_set.end(),
//                std::back_inserter(added));

//            newEvents.clear();

//            std::set_intersection(
//                m_visibleEvents.begin(),
//                m_visibleEvents.end(),
//                diff_set.begin(),
//                diff_set.end(),
//                std::back_inserter(remvd));

//            diff_set.clear();

//            for(auto idx : remvd)
//            {
//                auto it = std::find(
//                    m_visibleEvents.begin(), m_visibleEvents.end(), idx);
//                beginRemoveRows(
//                    {},
//                    it - m_visibleEvents.begin(),
//                    it - m_visibleEvents.begin());
//                m_visibleEvents.erase(it);
//                endRemoveRows();
//            }

//            for(auto idx : added)
//            {
//                auto it = std::find_if(
//                    m_visibleEvents.begin(),
//                    m_visibleEvents.end(),
//                    [idx](quint64 otherIdx) { return otherIdx > idx; });

//                beginInsertRows(
//                    {},
//                    it - m_visibleEvents.begin(),
//                    it - m_visibleEvents.begin());
//                m_visibleEvents.insert(it, idx);
//                endInsertRows();
//            }
//        },
//        Qt::DirectConnection);
}

int ThreadModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(m_eventMeta.size());
}

QVariant ThreadModel::data(const QModelIndex& index, int role) const
{
    auto eventIndex = m_eventMeta.at(index.row()).index;
    auto event      = m_meta.events->at(eventIndex).toObject();

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
        return m_eventMeta.at(index.row()).timestamp / m_meta.timestampDivisor;
    case FieldDuration:
        return m_eventMeta.at(index.row()).duration / m_meta.timestampDivisor;
    case FieldStackDepth:
        return m_eventMeta.at(index.row()).stackDepth;
    case FieldEventId:
        return index.row();
    default:
        return {};
    }
}

QHash<int, QByteArray> ThreadModel::roleNames() const
{
    return {
        {FieldType, "type"},
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

EventModel::EventModel(
    EventMetaData* meta,
    EventStats*    stats,
    TraceMetaData& traceMeta,
    QObject*       parent) :
    QObject(parent),
    m_id(meta->index), m_meta(meta), m_stats(stats),
    m_timestampDivisor(traceMeta.timestampDivisor)
{
    m_eventData = traceMeta.events->at(meta->index).toObject().toVariantMap();
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
    return m_meta->timestamp / m_timestampDivisor;
}

double EventModel::duration() const
{
    return m_meta->duration / m_timestampDivisor;
}

quint64 EventModel::id() const
{
    return m_id;
}

double EventModel::average() const
{
    return m_stats->average / m_timestampDivisor;
}

double EventModel::max() const
{
    return m_stats->max / m_timestampDivisor;
}

double EventModel::min() const
{
    return m_stats->min / m_timestampDivisor;
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
        &m_man,
        &QNetworkAccessManager::finished,
        this,
        [this](QNetworkReply* reply) {
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
        &QNetworkReply::errorOccurred,
        this,
        [this, reply](QNetworkReply::NetworkError) {
            emit fileError(reply->errorString());
        });
}

QString MetaData::graphicsApi() const
{
    return m_extra->value("extra")["graphics:library"].toString();
}
