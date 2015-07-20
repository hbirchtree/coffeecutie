#include "qscriptvectorvalue.h"

QString VectorData::toString()
{
    return QString("VectorData(size=%1 bytes,data=0x%2)")
            .arg(getVectorDataSize())
            .arg(QString::number((uintptr_t)_tmp_vec_storage,16));
}

VectorData::VectorData(QObject *parent):QObject(parent)
{
}
