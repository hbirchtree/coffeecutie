#include "qscriptvectorvalue.h"

const void *VectorData::getVectorData()
{
    updateVectorData();
    return _tmp_vec_storage;
}

QString VectorData::toString()
{
    return QString("VectorData(size=%1b,data=0x%2)")
            .arg(getVectorDataSize())
            .arg(QString::number((uintptr_t)_tmp_vec_storage,16));
}

void VectorData::bindSignals(VectorData *c)
{
    if(c)
        m_bindConnect = connect(this,&VectorData::valueChanged,[=](){
            c->valueChanged();
        });
    else
        disconnect(m_bindConnect);
}

VectorData::VectorData(QObject *parent):QObject(parent)
{
}
