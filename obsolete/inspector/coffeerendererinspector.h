#ifndef COFFEERENDERERINSPECTOR_H
#define COFFEERENDERERINSPECTOR_H

#include <QWidget>
#include "graphplotter.h"
#include <QPainter>
#include <QTreeWidgetItem>

namespace Ui {
class CoffeeRendererInspector;
}

class CoffeeRendererInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeRendererInspector(QWidget *parent);
    ~CoffeeRendererInspector();

public slots:
    void setSysInfoField(const QString& field, const QString& data);
    void plotGraph(float frametime);

private slots:
    void on_plotResizer_valueChanged(int value);

private:
    GraphPlotter* scene = nullptr;
    Ui::CoffeeRendererInspector *ui;

    QHash<QString,QTreeWidgetItem*> m_properties;
};

#endif // COFFEERENDERERINSPECTOR_H
