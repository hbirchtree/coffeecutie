#ifndef COFFEEDATACONTAINER_H
#define COFFEEDATACONTAINER_H

#include <QObject>
#include <QHash>
#include <QString>

template <class K,class V> class CoffeeDataContainer : public QObject
{
public:
    CoffeeDataContainer(QObject *parent) : QObject(parent)
    {

    }

    V getValue(K key)
    {
        return data.value(key);
    }

    void setValue(K key, V value)
    {
        data.insert(key,value);
    }


private:
    QHash<K,V> data;
};

#endif // COFFEEDATACONTAINER_H
