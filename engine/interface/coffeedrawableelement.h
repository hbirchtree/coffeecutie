#ifndef COFFEEDRAWABLEELEMENT_H
#define COFFEEDRAWABLEELEMENT_H

#include "general/common.h"
#include "engine/objects/coffeeobject.h"

class CoffeeTexture;
class CoffeeShader;
class Vector2Value;

class CoffeeDrawableElement : public QObject,public CoffeeObject
{
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QObject* texture READ texture WRITE setTexture)
    Q_PROPERTY(QObject* shader READ shader WRITE setShader)
    Q_PROPERTY(QObject* position READ positionQObject WRITE setPosition)
    Q_PROPERTY(QObject* size READ size WRITE setSize)

    Q_OBJECT

    Vector2Value* m_size;
    Vector2Value* m_position;
    QPointer<CoffeeShader> m_shader;
    QPointer<CoffeeTexture> m_texture;

    QString m_text;

public:
    CoffeeDrawableElement(QObject *parent);

    QObject* size();
    QObject* positionQObject();
    QObject* shader();
    QObject* texture();

    CoffeeTexture* getTexture();
    CoffeeShader* getShader();

    QString text() const;

public slots:
    void setSize(QObject* size);
    void setPosition(QObject* position);
    void setShader(QObject* shader);
    void setTexture(QObject* texture);
    void setText(const QString &text);

    void render();
    void unload();
    void load();

};

#endif // COFFEEDRAWABLEELEMENT_H
