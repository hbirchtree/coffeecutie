#ifndef COFFEEINSPECTOR_H
#define COFFEEINSPECTOR_H

#include <QWidget>
#include <opengl/context/coffeerenderer.h>
#include "coffeerendererinspector.h"
#include <QTreeWidgetItem>
#include <QMetaProperty>
#include <QList>
#include <QPointer>
#include <QHash>

namespace Ui {
class CoffeeInspector;
}

class CoffeeInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeInspector(QWidget *parent = 0,QObject* engineRoot = nullptr,CoffeeRenderer* renderer = nullptr);
    ~CoffeeInspector();

private slots:
    void updateInformation();
    void on_updateBtn_clicked();

    void on_rendererBtn_clicked();

private:
    QHash<QObject*,QTreeWidgetItem*> objectsMapping;
    QHash<QObject*,QTreeWidgetItem*> childTrees; //Yup. So far the weirdest name I've come up with. I'm keeping it.
//    QHash<QMetaProperty,QTreeWidgetItem*> propertyMapping; //One property can only exist in one place. This seems fair enough.
    QHash<QObject*,QHash<int,QTreeWidgetItem*>> propertyMapping; //We assume that properties stay the same, only change value

    QPointer<QObject> engineRoot;

    QPointer<CoffeeRenderer> renderer;
    QPointer<CoffeeRendererInspector> rendererInspector;

    Ui::CoffeeInspector *ui;
    void updateProperties(QObject* object);
    void updateTreeWidgetItem(QObject* object, QTreeWidgetItem *parent);
    void updateProperty(QTreeWidgetItem* it, QVariant value);
    void clearChildren(QTreeWidgetItem* it);
};

#endif // COFFEEINSPECTOR_H
