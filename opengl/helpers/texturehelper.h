#ifndef TEXTUREHELPER_H
#define TEXTUREHELPER_H

#include <QObject>

class TextureHelper : public QObject
{
    Q_OBJECT
public:
    explicit TextureHelper(QObject *parent = 0);
    ~TextureHelper();

signals:

public slots:
};

#endif // TEXTUREHELPER_H
