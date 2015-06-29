#ifndef COFFEERESOURCE_H
#define COFFEERESOURCE_H

#include <QObject>

class CoffeeResourceError{
public:
    enum ResourceError {
        Undefined,
        MissingFileError,
        InvalidUrlError,
        UnreadableFileError
    };
    explicit CoffeeResourceError(ResourceError e = CoffeeResourceError::Undefined);
    ResourceError type();
private:
    ResourceError t;
};

class CoffeeResource : public QObject
{
    Q_PROPERTY(bool remote READ remote NOTIFY remoteChanged) //Resources should work regardless of being local or remote
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY resourceChanged)
    Q_PROPERTY(ResourceType type READ type NOTIFY typeChanged)

    Q_OBJECT
    QString m_source;

public:
    enum ResourceType {
        None, //Basically a null-resource
        BinaryFile, //Formats which are not to be read directly by the engine. For instance: textures, models, sounds, etc.
        ObjectNotation, //JSON, imported directly to a QVariantMap for use in script environment
        AssetIndex, //For importing assets. This index might change.
        TextFile, //If you want to do something saucy with pure-text processing.
        Streaming //We'll find out if this is possible
    };
    Q_ENUMS(ResourceType)

    CoffeeResource(QObject *parent, QString source);

    QString source() const;
    ResourceType type() const;
    bool remote() const;

    QByteArray* data();

signals:
    void resourceChanged(); //new file or file changed
    void resourceSlow();
    void resourceLoaded(); //if resource is slow, this is used to indicate that it is done.

    void typeChanged(ResourceType type);
    void remoteChanged(bool remote);

public slots:
    void refresh();

    void setSource(QString source);
    void setType(ResourceType type);
    void setRemote(bool remote);

private:
    static bool checkSource(QString u);

    bool loadLocalFile();
    bool loadRemoteFile();

    QByteArray m_data;
    ResourceType m_type = None;
    bool m_remote = false;
};

#endif // COFFEERESOURCE_H
