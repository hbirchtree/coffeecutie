#ifndef FILEHANDLER
#define FILEHANDLER

class QStringList;
class QString;
class QByteArray;

class FileHandler {
public:
    static QStringList getStringListFromFile(QString file);
    static QString getStringFromFile(QString file);
    static unsigned char *getDataFromFile(QString file);
    static QByteArray* getBytesFromFile(QString file);
};

#endif // FILEHANDLER

