#ifndef COFFEEOBJECTEXPLORER_H
#define COFFEEOBJECTEXPLORER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QVariant>

class CoffeeObjectExplorer : public QObject
{
public:
    CoffeeObjectExplorer(QObject* parent,
                         QObjectList root);

    void setTarget(QTreeWidget *target);

public slots:
    void addObject(QObject* obj);
    void updateInformation();

protected:
    void updateTreeWidgetItem(QObject *object,
                              QTreeWidgetItem *parent,
                              bool listChildren = true);
    void updateProperties(QObject *object);
    void updateProperty(QTreeWidgetItem *it,
                        QVariant value);
    void clearChildren(QTreeWidgetItem *it);
private:
    QObjectList m_root;
    QTreeWidget* m_target;

    QHash<QObject*,QTreeWidgetItem*> objectsMapping;
    QHash<QObject*,QTreeWidgetItem*> childTrees;
    QHash<QObject*,QHash<int,QTreeWidgetItem*>> propertyMapping;
};

#endif // COFFEEOBJECTEXPLORER_H
