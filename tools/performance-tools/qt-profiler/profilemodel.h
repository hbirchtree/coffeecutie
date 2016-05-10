#pragma once

#include <QStandardItemModel>

#include <coffee/core/CXmlParser>

using namespace Coffee;

const constexpr cstring token_thread = "thread";
const constexpr cstring token_label = "label";
const constexpr cstring token_timestamp = "ts";

const constexpr cstring token_context = "context";
const constexpr cstring token_datapoint = "dpoint";

const constexpr int count_columns = 5;

QStandardItem* ParseDataPoints(XML::Element* point, uint64* cost = nullptr)
{
    QStandardItem* d = new QStandardItem();

    d->setData(point->Attribute(token_thread),1);
    d->setData(point->Attribute(token_label),2);
    d->setData(point->Attribute(token_timestamp),3);

    XML::Element* c = point->FirstChildElement();
    while(c)
    {
        if(StrCmp(c->Name(),token_datapoint))
        {
            QStandardItem* v = new QStandardItem;
            v->setData(c->Attribute(token_thread),1);
            v->setData(c->Attribute(token_label),2);
            quint64 t = QString(c->Attribute(token_timestamp)).toULongLong();
            if(cost)
                (*cost) += t;
            v->setData(t,3);
        }else if(StrCmp(c->Name(),token_context))
        {
            d->setChild(d->rowCount(),ParseDataPoints(c));
        }
        c = point->NextSiblingElement();
    }
    return d;
}

QStandardItemModel* GenerateProfileTree(XML::Element* datapoints, QObject* parent = nullptr)
{
    QStandardItemModel* mod = new QStandardItemModel(parent);

    mod->setColumnCount(count_columns);

    mod->setHeaderData(0,Qt::Horizontal,QVariant("TID"));
    mod->setHeaderData(1,Qt::Horizontal,QVariant("Blargh"));

    int i=0;
    XML::Element* el = datapoints->FirstChildElement();
    while(el)
    {
        mod->setItem(i,ParseDataPoints(el));
        el = datapoints->NextSiblingElement();
    }

    return mod;
}
