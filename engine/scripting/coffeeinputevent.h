#ifndef COFFEEINPUTEVENT_H
#define COFFEEINPUTEVENT_H

#include "general/common.h"

class CoffeeInputEvent : public QObject
{
    Q_PROPERTY(EventSource type READ type WRITE setType)
    Q_PROPERTY(QVariantList data READ data WRITE setData)

    Q_OBJECT
public:

    //A quite simplified model of input
    enum EventSource {
        None,

        InputMethod,

        Mouse,Keyboard,

        Gamepad,

        Touch //got nuthin'
    };
    Q_ENUMS(EventSource)

    CoffeeInputEvent(QObject *parent);
    CoffeeInputEvent(QObject *parent, const QEvent &ev); //QEvent is not available in QtScript apparently

    EventSource type() const;
    QVariantList data() const;

public slots:
    void setType(EventSource type);
    void setData(QVariantList data);

    void accept(); //destroys the object

private:
    EventSource m_type = None;
    QVariantList m_data;
};

#endif // COFFEEINPUTEVENT_H
