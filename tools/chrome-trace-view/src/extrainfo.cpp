#include "extrainfo.h"

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

QJsonObject ExtraInfo::value(QString const& name) const
{
    auto it = std::find_if(
        m_groups.begin(),
        m_groups.end(),
        [&name](std::unique_ptr<ExtraInfoGroup> const& g) {
            return g->name() == name;
        });

    if(it == m_groups.end())
        return {};

    return (*it)->extraData();
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

void ExtraInfo::updateData(QJsonObject const& trace)
{
    beginRemoveRows({}, 0, m_groups.size());
    m_groups.clear();
    endRemoveRows();

    std::vector<QString> selectedKeys;

    auto keys = trace.keys();
    for(auto val : keys)
        if(isProfileInfo(val))
            selectedKeys.push_back(val);

    beginInsertRows({}, 0, selectedKeys.size());
    for(auto val : keys)
    {
        if(!isProfileInfo(val))
            continue;

        m_groups.push_back(
            std::make_unique<ExtraInfoGroup>(val, trace.find(val)->toObject()));
    }

    endInsertRows();

    emit updated();
}

ExtraInfoGroup::ExtraInfoGroup(
    const QString& name, QJsonObject const& data, QObject* parent) :
    QAbstractListModel(parent),
    m_data(data), m_name(name)
{
    auto keys = data.keys();
    for(auto const& key : keys)
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
