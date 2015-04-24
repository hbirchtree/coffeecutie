#ifndef COFFEEMATERIAL
#define COFFEEMATERIAL

#include <QString>
#include "general/floatcontainer.h"
#include "glm/vec3.hpp"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
class CoffeeMaterial{
    Q_PROPERTY(QString bumpTexture READ bumpTexture WRITE setBumpTexture NOTIFY bumpTextureChanged)
    Q_PROPERTY(QString diffuseTexture READ diffuseTexture WRITE setDiffuseTexture NOTIFY diffuseTextureChanged)
    Q_PROPERTY(QString specularTexture READ specularTexture WRITE setSpecularTexture NOTIFY specularTextureChanged)
    Q_PROPERTY(QString highlightTexture READ highlightTexture WRITE setHighlightTexture NOTIFY highlightTextureChanged)
    Q_PROPERTY(QString transparencyTexture READ transparencyTexture WRITE setTransparencyTexture NOTIFY transparencyTextureChanged)

    Q_PROPERTY(GLint bumpHandle READ bumpHandle WRITE setBumpHandle NOTIFY bumpHandleChanged)
    Q_PROPERTY(GLint diffuseHandle READ diffuseHandle WRITE setDiffuseHandle NOTIFY diffuseHandleChanged)
    Q_PROPERTY(GLint specularHandle READ specularHandle WRITE setSpecularHandle NOTIFY specularHandleChanged)
    Q_PROPERTY(GLint highlightHandle READ highlightHandle WRITE setHighlightHandle NOTIFY highlightHandleChanged)
    Q_PROPERTY(GLint transparencyHandle READ transparencyHandle WRITE setTransparencyHandle NOTIFY transparencyHandleChanged)

    Q_PROPERTY(FloatContainer transparency READ transparency WRITE setTransparency NOTIFY transparencyChanged)
    Q_PROPERTY(FloatContainer shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

    Q_PROPERTY(glm::vec3 specularColor READ specularColor WRITE setSpecularColor NOTIFY specularColorChanged)
    Q_PROPERTY(glm::vec3 colorMultiplier READ colorMultiplier WRITE setColorMultiplier NOTIFY colorMultiplierChanged)

public:
    QString bumpTexture() const
    {
        return m_bumpTexture;
    }

    QString diffuseTexture() const
    {
        return m_diffuseTexture;
    }

    QString highlightTexture() const
    {
        return m_highlightTexture;
    }

    QString specularTexture() const
    {
        return m_specularTexture;
    }

    QString transparencyTexture() const
    {
        return m_transparencyTexture;
    }

    GLint bumpHandle() const
    {
        return m_bumpHandle;
    }

    GLint diffuseHandle() const
    {
        return m_diffuseHandle;
    }

    GLint specularHandle() const
    {
        return m_specularHandle;
    }

    GLint highlightHandle() const
    {
        return m_highlightHandle;
    }

    GLint transparencyHandle() const
    {
        return m_transparencyHandle;
    }

    QPointer<FloatContainer> transparency()
    {
        return &m_transparency;
    }

    QPointer<FloatContainer> shininess()
    {
        return &m_shininess;
    }

    QPointer<glm::vec3> specularColor()
    {
        return &m_specularColor;
    }

    QPointer<glm::vec3> colorMultiplier()
    {
        return &m_colorMultiplier;
    }

public slots:
    void setBumpTexture(QString arg)
    {
        if (m_bumpTexture == arg)
            return;

        m_bumpTexture = arg;
        emit bumpTextureChanged(arg);
    }

    void setDiffuseTexture(QString arg)
    {
        if (m_diffuseTexture == arg)
            return;

        m_diffuseTexture = arg;
        emit diffuseTextureChanged(arg);
    }

    void setHighlightTexture(QString arg)
    {
        if (m_highlightTexture == arg)
            return;

        m_highlightTexture = arg;
        emit highlightTextureChanged(arg);
    }

    void setSpecularTexture(QString arg)
    {
        if (m_specularTexture == arg)
            return;

        m_specularTexture = arg;
        emit specularTextureChanged(arg);
    }

    void setTransparencyTexture(QString arg)
    {
        if (m_transparencyTexture == arg)
            return;

        m_transparencyTexture = arg;
        emit transparencyTextureChanged(arg);
    }

    void setBumpHandle(GLint arg)
    {
        if (m_bumpHandle == arg)
            return;

        m_bumpHandle = arg;
        emit bumpHandleChanged(arg);
    }

    void setDiffuseHandle(GLint arg)
    {
        if (m_diffuseHandle == arg)
            return;

        m_diffuseHandle = arg;
        emit diffuseHandleChanged(arg);
    }

    void setSpecularHandle(GLint arg)
    {
        if (m_specularHandle == arg)
            return;

        m_specularHandle = arg;
        emit specularHandleChanged(arg);
    }

    void setHighlightHandle(GLint arg)
    {
        if (m_highlightHandle == arg)
            return;

        m_highlightHandle = arg;
        emit highlightHandleChanged(arg);
    }

    void setTransparencyHandle(GLint arg)
    {
        if (m_transparencyHandle == arg)
            return;

        m_transparencyHandle = arg;
        emit transparencyHandleChanged(arg);
    }

    void setTransparency(FloatContainer arg)
    {
        if (m_transparency == arg)
            return;

        m_transparency = arg;
        emit transparencyChanged(arg);
    }

    void setShininess(FloatContainer arg)
    {
        if (m_shininess == arg)
            return;

        m_shininess = arg;
        emit shininessChanged(arg);
    }

    void setSpecularColor(glm::vec3 arg)
    {
        if (m_specularColor == arg)
            return;

        m_specularColor = arg;
        emit specularColorChanged(arg);
    }

    void setColorMultiplier(glm::vec3 arg)
    {
        if (m_colorMultiplier == arg)
            return;

        m_colorMultiplier = arg;
        emit colorMultiplierChanged(arg);
    }

signals:
    void bumpTextureChanged(QString arg);

    void diffuseTextureChanged(QString arg);

    void highlightTextureChanged(QString arg);

    void specularTextureChanged(QString arg);

    void transparencyTextureChanged(QString arg);

    void bumpHandleChanged(GLint arg);

    void diffuseHandleChanged(GLint arg);

    void specularHandleChanged(GLint arg);

    void highlightHandleChanged(GLint arg);

    void transparencyHandleChanged(GLint arg);

    void transparencyChanged(FloatContainer arg);

    void shininessChanged(FloatContainer arg);

    void specularColorChanged(glm::vec3 arg);

    void colorMultiplierChanged(glm::vec3 arg);

private:
    QString m_bumpTexture;
    QString m_diffuseTexture;
    QString m_highlightTexture;
    QString m_specularTexture;
    QString m_transparencyTexture;
    GLint m_bumpHandle;
    GLint m_diffuseHandle;
    GLint m_specularHandle;
    GLint m_highlightHandle;
    GLint m_transparencyHandle;
    FloatContainer m_transparency;
    FloatContainer m_shininess;
    glm::vec3 m_specularColor;
    glm::vec3 m_colorMultiplier;
};

#endif // COFFEEMATERIAL

