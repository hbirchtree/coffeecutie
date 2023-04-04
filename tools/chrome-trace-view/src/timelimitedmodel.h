#pragma once

#include <QSortFilterProxyModel>
#include <QTimer>

#include <functional>

#include "shared_fields.h"

class TimeLimitedModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* model READ model WRITE setModel NOTIFY sourceModelChanged)
    Q_PROPERTY(double viewStart MEMBER m_viewStart READ viewStart NOTIFY viewStartChanged)
    Q_PROPERTY(double viewEnd MEMBER m_viewEnd READ viewEnd NOTIFY viewEndChanged)
  public:
    TimeLimitedModel(QObject* parent = nullptr);

    QObject* model() const;
    void setModel(QObject* model);

    double viewStart() const { return m_viewStart; }
    double viewEnd() const { return m_viewEnd; }

  signals:
    void sourceModelChanged();
    void viewStartChanged();
    void viewEndChanged();
    void timestampFieldChanged();

  protected slots:
    void invalidateAfterDebounce();

  protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
  private:
    QTimer m_debounce;
    double m_viewStart{0.0};
    double m_viewEnd{0.0};
};
