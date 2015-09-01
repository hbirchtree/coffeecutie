#ifndef QSCRIPTVECTORVALUE_H
#define QSCRIPTVECTORVALUE_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

//If we ever decide to switch to using doubles at some point
typedef float ScalarDataType;

class VectorData : public QObject
{
    Q_OBJECT
public:
    virtual void updateVectorData() = 0;
    virtual const void* getVectorData();
    virtual uint32_t getVectorDataSize() const = 0;

    Q_INVOKABLE virtual QString toString();

signals:
    void valueChanged();

protected:
    void bindSignals(VectorData* c);
    VectorData(QObject*parent);
    void* _tmp_vec_storage = nullptr;

private:
    QMetaObject::Connection m_bindConnect;
};

#include "engine/data/vectors/vector2value.h"
#include "engine/data/vectors/vector3value.h"
#include "engine/data/vectors/vector4value.h"
#include "engine/data/vectors/matrix4value.h"
#include "engine/data/vectors/quatvalue.h"
#include "engine/data/vectors/scalarvalue.h"

#endif // QSCRIPTVECTORVALUE_H
