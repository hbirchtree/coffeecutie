#pragma once

#include <QAbstractListModel>
#include <QJsonObject>
#include <memory>

class TraceModel;

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

    QJsonObject extraData() const
    {
        return m_data;
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

    QJsonObject value(QString const& name) const;

  public slots:
    void updateData(QJsonObject const& trace);
  signals:
    void updated();
};
