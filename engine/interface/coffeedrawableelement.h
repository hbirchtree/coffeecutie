#ifndef COFFEEDRAWABLEELEMENT_H
#define COFFEEDRAWABLEELEMENT_H

#include "general/common.h"

class CoffeeTexture;
class CoffeeShader;
class Vector3Value;

class CoffeeDrawableElement : public QObject
{
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QObject* texture READ texture WRITE setTexture)
    Q_PROPERTY(QObject* shader READ shader WRITE setShader)
    Q_PROPERTY(QObject* position READ position WRITE setPosition)
    Q_PROPERTY(QObject* size READ size WRITE setSize)

    Q_OBJECT

    Vector3Value* m_size;
    Vector3Value* m_position;
    CoffeeShader* m_shader;
    CoffeeTexture* m_texture;

    QString m_text;

public:
    CoffeeDrawableElement(QObject *parent);

    QObject* size();
    QObject* position();
    QObject* shader();
    QObject* texture();
    QString text() const;

public slots:
    void setSize(QObject* size);
    void setPosition(QObject* position);
    void setShader(QObject* shader);
    void setTexture(QObject* texture);
    void setText(const QString &text);
};

#endif // COFFEEDRAWABLEELEMENT_H
