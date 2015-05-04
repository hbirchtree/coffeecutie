#ifndef MUTABLEDATACONTAINER_H
#define MUTABLEDATACONTAINER_H

#include <QObject>

template <class T> class MutableDataContainer : public QObject
{
    //Used for ShaderVariant in order to store data in QObject* while not wasting memory (as if this does not waste memory)
    //Primarily made to store std::function
public:
    MutableDataContainer(QObject *parent,T value) : QObject(parent){
        data = value;
    }

    void setValue(T data){
        this->data = data;
    }
    T getValue(){
        return data;
    }

private:
    T data;
};

#endif // MUTABLEDATACONTAINER_H
