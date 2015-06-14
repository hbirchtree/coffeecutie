#ifndef COFFEEINSPECTOR_H
#define COFFEEINSPECTOR_H

#include <QWidget>
#include <opengl/context/coffeerenderer.h>
#include "coffeerendererinspector.h"
#include "coffeescriptterminal.h"
#include <QTreeWidgetItem>
#include <QMetaProperty>
#include <QList>
#include <QTimer>
#include <QPointer>
#include <QHash>
#include <QRunnable>

namespace Ui {
class CoffeeInspector;
}

class CoffeeInspector : public QWidget, public QRunnable
{
    Q_OBJECT

public:
    CoffeeInspector(QWidget *parent = 0,QList<QObject*> engineRoot = QList<QObject*>(),CoffeeRenderer* renderer = nullptr);
    ~CoffeeInspector();

private slots:
    void updateInformation();

    void on_rendererBtn_clicked();

    void on_pushButton_clicked();

private:
    QTimer *refreshTimer;

    QTreeWidgetItem* threadInfoItem = nullptr;
    QHash<QObject*,QTreeWidgetItem*> objectsMapping;
    QHash<QObject*,QTreeWidgetItem*> childTrees; //Yup. So far the weirdest name I've come up with. I'm keeping it.
//    QHash<QMetaProperty,QTreeWidgetItem*> propertyMapping; //One property can only exist in one place. This seems fair enough.
    QHash<QObject*,QHash<int,QTreeWidgetItem*>> propertyMapping; //We assume that properties stay the same, only change value

    QList<QObject*> engineRoot;

    QPointer<CoffeeRenderer> renderer;
    QPointer<CoffeeRendererInspector> rendererInspector;
    QPointer<CoffeeScriptTerminal> scriptTerminal;

    Ui::CoffeeInspector *ui;
    void updateProperties(QObject* object);
    void updateTreeWidgetItem(QObject* object, QTreeWidgetItem *parent, bool listChildren = true);
    void updateProperty(QTreeWidgetItem* it, QVariant value);
    void clearChildren(QTreeWidgetItem* it);


    // QRunnable interface
public:
    void run();
};

#endif // COFFEEINSPECTOR_H
