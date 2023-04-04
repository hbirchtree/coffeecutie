#include "metrics.h"

#include <QJsonObject>

MetricValues::MetricValues(
    Metrics* metrics, TraceMetaData& meta, QObject* parent) :
    QAbstractListModel(parent),
    m_meta(meta)
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
    return m_values.size();
}

QVariant MetricValues::data(const QModelIndex& index, int role) const
{
    if(index.row() >= m_values.size())
        return {};

    switch(role)
    {
    case FieldValue:
        return m_values.at(index.row()).value;
    case FieldIndex:
        return m_values.at(index.row()).i;
    case FieldValueScaled:
        return metric_value_scale(
            m_values.at(index.row()).value, m_minValue, m_maxValue);

    case FieldTimestamp:
        return m_values.at(index.row()).ts / m_meta.timestampDivisor;

    case FieldPreviousValue:
        return index.row() < 1 ? 0.f : m_values.at(index.row() - 1).value;
    case FieldPreviousValueScaled:
        return index.row() < 1
                   ? 0.f
                   : metric_value_scale(
                         m_values.at(index.row() - 1).value, m_minValue, m_maxValue);

    case FieldPreviousTimestamp:
        return index.row() < 1 ? 0 : m_values.at(index.row() - 1).ts / m_meta.timestampDivisor;
    default:
        return {};
    }
}

QHash<int, QByteArray> MetricValues::roleNames() const
{
    return {
        {FieldValue, "value"},
        {FieldIndex, "eventId"},
        {FieldValueScaled, "valueScaled"},
        {FieldTimestamp, "ts"},
        {FieldPreviousValue, "previousValue"},
        {FieldPreviousValueScaled, "previousValueScaled"},
        {FieldPreviousTimestamp, "previousTs"},
    };
}

MetricPhase::MetricPhase(QObject* parent) : QAbstractListModel(parent)
{
}

int MetricPhase::rowCount(const QModelIndex&) const
{
    return m_list.size();
}

QVariant MetricPhase::data(const QModelIndex& index, int role) const
{
    MetricValues* values = m_list.at(index.row()).get();

    switch(role)
    {
    case FieldPhase:
        return QVariant::fromValue(values);
    case FieldColor:
        return "green";
    case FieldIndex:
        return 0;
    }

    return {};
}

QHash<int, QByteArray> MetricPhase::roleNames() const
{
    return {
        {FieldPhase, "phase"}, {FieldColor, "color"}, {FieldIndex, "index"}};
}

QVariantList MetricPhase::sampleValue(double x)
{
    QVariantList out;
    for(auto const& phase : m_list)
        out.push_back(phase->sampleValue(x));
    return out;
}

Metrics::Metrics(TraceMetaData& meta, QObject* parent) :
    QAbstractListModel(parent), m_meta(meta)
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
    case FieldNumPhases:
        return QVariant::fromValue(m_metricList.at(index.row())->m_list.size());
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
        {FieldNumPhases, "numPhases"},
    };
}

void Metrics::insertValue(const QJsonObject& data, quint64 i)
{
    auto id_it    = data.find("id");
    auto idx_it   = data.find("i");
    auto value_it = data.find("v");
    auto ts_it    = data.find("ts");

    if(id_it == data.end() || value_it == data.end() || ts_it == data.end())
        return;

    auto id = (*id_it).toInt();

    auto metric_it = m_metric.find(id);

    if(metric_it == m_metric.end())
        return;

    auto metric = metric_it->second;

    auto idx      = idx_it != data.end() ? idx_it->toInt() : 0;
    auto phase_it = metric->m_values.find(idx);

    if(phase_it == metric->m_values.end())
    {
        auto values = std::make_shared<MetricValues>(this, m_meta, this);
        auto new_it = metric->m_values.insert({idx, values});
        metric->m_list.push_back(values);
        phase_it = new_it.first;
    }

    auto phase = phase_it->second;

    auto ts = ts_it->toVariant().toString().toULongLong() - m_meta.firstTimestamp;

    phase->m_values.push_back({ts, value_it->toString().toFloat(), i});

    auto& newValue    = phase->m_values.back();
    phase->m_minValue = std::min(phase->m_minValue, newValue.value);
    phase->m_maxValue = std::max(phase->m_maxValue, newValue.value);

    phase->m_numEvents++;

    auto diff = ts - phase->m_prevTime;
    phase->m_average += diff;
    phase->m_prevTime = ts;
}

void Metrics::populate(const QJsonObject& meta)
{
    auto args_it = meta.find("args");
    auto id_it   = meta.find("id");

    if(args_it == meta.end() || id_it == meta.end())
        return;

    auto args = args_it->toObject().toVariantHash();
    auto id   = id_it->toInt();

    auto metric = std::make_shared<MetricValues>(this, m_meta, this);

    auto phase    = std::make_shared<MetricPhase>(this);
    phase->m_name = args["name"].toString();
    phase->m_id   = id;

    switch(args["type"].toInt())
    {
    case 1:
        phase->m_type = MetricPhase::MetricSymbolic;
        break;
    case 2:
        phase->m_type = MetricPhase::MetricMarker;
        break;
    case 3:
        phase->m_type = MetricPhase::MetricImage;
        break;
    default:
        phase->m_type = MetricPhase::MetricValue;
        break;
    }
    phase->m_template = metric;

    m_metric.insert({id, phase});
    m_metricList.push_back(phase);
}

void Metrics::optimize()
{
    beginResetModel();
    endResetModel();
    beginInsertRows({}, 0, m_metricList.size());
    endInsertRows();
    for(auto& metric : m_metricList)
        for(auto& phase : metric->m_values)
        {
            auto& values = *phase.second;
            values.beginInsertRows({}, 0, values.m_values.size());

            values.m_average /= values.m_numEvents;

            if(metric->m_type != MetricPhase::MetricValue)
                continue;

            if((values.m_maxValue - values.m_minValue) < 1.f / 1000000.f)
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

            /* Remove duplicate values */
            for(size_t i = 0; i < values.m_values.size(); i++)
            {
                auto current = values.m_values.at(i).value;
                if(previous == current)
                    to_remove.push_back(i);
            }

            for(auto it = to_remove.rbegin(); it != to_remove.rend(); ++it)
            {
                if(*it == (values.m_values.size() - 1) || *it == 0)
                    continue;

                values.m_values.erase(values.m_values.begin() + *it);
            }
            values.endInsertRows();
        }
}

void Metrics::updateView()
{
    //    for(auto const& metricGroup : m_metricList)
    //        for(auto const& phase : metricGroup->m_list)
    //        {
    //            auto& metric = *phase;
    //            metric.beginInsertRows({}, 0, metric.m_values.size());
    //            metric.endInsertRows();
    //        }
    /*
        auto start = m_trace->m_viewStart, end = m_trace->m_viewEnd;

        for(auto const& metric_ : m_metricList)
        {
            for(auto const& phase_ : metric_->m_list)
            {
                auto& metric = *phase_;
                auto& values = metric.m_values;

                std::vector<quint64> visible;

                quint64 i = 0;
                for(auto const& v : values)
                {
                    auto ev_start = i > 0 ? values[i - 1].ts : 0.0, ev_end =
       v.ts;

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
                    auto it = std::find(
                        metric.m_visible.begin(), metric.m_visible.end(), i);
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
    */
}
