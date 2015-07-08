#ifndef COFFEECOMPUTERESOURCE_H
#define COFFEECOMPUTERESOURCE_H

#include "general/common.h"
class CoffeeComputeResource : public QObject
{
    Q_PROPERTY(ComputeType type READ type)
    Q_PROPERTY(GLuint transformFeedback READ transformFeedback WRITE setTransformFeedback)
public:
    enum ComputeType {
        OpenCL_ComputeOutput,
        OpenGL_TransformFeedback
    };

    CoffeeComputeResource(QObject* parent);
    ComputeType type() const;
private:
    ComputeType m_type;
};

#endif // COFFEECOMPUTERESOURCE_H
