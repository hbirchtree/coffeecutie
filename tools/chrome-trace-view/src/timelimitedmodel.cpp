#include "timelimitedmodel.h"

#include <QDebug>

TimeLimitedModel::TimeLimitedModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    m_debounce.setInterval(200);
    m_debounce.setSingleShot(true);

    setFilterRole(TIMESTAMP_ROLE);

    connect(
        this,
        &TimeLimitedModel::viewStartChanged,
        this,
        &TimeLimitedModel::invalidateAfterDebounce);
    connect(
        this,
        &TimeLimitedModel::viewEndChanged,
        this,
        &TimeLimitedModel::invalidateAfterDebounce);
    connect(this, &TimeLimitedModel::viewStartChanged, this, [this] {
        qDebug() << "Range" << m_viewStart << m_viewEnd;
    });
    connect(&m_debounce, &QTimer::timeout, this, &TimeLimitedModel::invalidateFilter);
}

QObject* TimeLimitedModel::model() const
{
    return sourceModel();
}

void TimeLimitedModel::setModel(QObject* model)
{
    if(model == sourceModel())
        return;

    setSourceModel(qobject_cast<QAbstractItemModel*>(model));
    Q_EMIT sourceModelChanged();
}

void TimeLimitedModel::invalidateAfterDebounce()
{
    qDebug() << "Current count: " << rowCount();
    m_debounce.start();
}

bool TimeLimitedModel::filterAcceptsRow(
    int source_row, const QModelIndex& source_parent) const
{
    auto model = sourceModel();

    bool   ok, okDuration;
    double timestamp =
        model->data(model->index(source_row, 0, source_parent), TIMESTAMP_ROLE).toDouble(&ok);
    double duration =
        model->data(model->index(source_row, 0, source_parent), DURATION_ROLE).toDouble(&okDuration);

    if(!ok)
        return false;

    double end = okDuration ? timestamp + duration : timestamp;

    qDebug() << m_viewStart << timestamp << m_viewEnd;

    return
        // Overlaps entirely
        (timestamp < m_viewStart && end > m_viewEnd) ||
        // Start is inside
        (timestamp > m_viewStart && timestamp < m_viewEnd) ||
        // End is inside
        (end > m_viewStart && end < m_viewEnd);
}
