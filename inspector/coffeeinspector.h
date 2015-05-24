#ifndef COFFEEINSPECTOR_H
#define COFFEEINSPECTOR_H

#include <QWidget>
#include <opengl/context/coffeerenderer.h>
#include <QTreeWidgetItem>
#include <QMetaProperty>
#include <QList>
#include <QPointer>

namespace Ui {
class CoffeeInspector;
}

class CoffeeInspector : public QWidget
{
    Q_OBJECT

public:
    CoffeeInspector(QWidget *parent = 0,QObject* engineRoot = nullptr);
    ~CoffeeInspector();

private slots:
    void updateInformation();
    void on_updateBtn_clicked();

private:
    QPointer<QObject> engineRoot;
    Ui::CoffeeInspector *ui;
    void populateTreeWidgetItem(QObjectList source, QTreeWidgetItem *target);
    QTreeWidgetItem* generateItem(QObject* o);
    QList<QTreeWidgetItem *> getProperties(QObject* object);
};

#endif // COFFEEINSPECTOR_H
