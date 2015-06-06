#ifndef NUMBERBUFFER_H
#define NUMBERBUFFER_H

#include "general/common.h"
template <class T> class NumberBuffer : public QObject
{
public:
    static NumberBuffer* createArray(QObject* parent,int size){
        return (new NumberBuffer(parent,size));
    }

    NumberBuffer* put(T *input)
    {
        int insize = sizeof(input)/sizeof(T);
        if(insize+pointer>=getSize())
            throw std::out_of_range("err: numberbuffer overflow");

        for(int i=0;i<insize;i++)
            put(input[i]);

        return this;
    }

    NumberBuffer* put(T in)
    {
        if(pointer>=getSize())
            throw std::out_of_range("err: numberbuffer overflow");
        data.insert(data.begin()+pointer,in);
        pointer++;

        return this;
    }

    void reset()
    {
        pointer = 0;
    }

    T* get()
    {
        return data.data();
    }

    int getSize()
    {
        return data.capacity();
    }

    QString toString()
    {
        QString res;
        return bufferToString(data.data(),0,getSize());
    }

    static QString bufferToString(T* data, int from, int to)
    {
        QString res;
        for(int i=from;i<to;i++)
            res.append(QString::number(data[i])+", ");
        return res;
    }

private:
    NumberBuffer(QObject* parent,int size):QObject(parent){
        data.reserve(size);
    }
    int pointer = 0;
    std::vector<T> data;
};

#endif // NUMBERBUFFER_H
