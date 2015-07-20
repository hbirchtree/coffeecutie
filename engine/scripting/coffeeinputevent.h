#ifndef COFFEEINPUTEVENT_H
#define COFFEEINPUTEVENT_H

#include "general/common.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class CoffeeInputEvent : public QObject
{
    Q_PROPERTY(EventSource type READ type WRITE setType)

    Q_PROPERTY(QEvent::Type qtype READ qtype)

    Q_PROPERTY(JoystickEventType jType READ jType WRITE setJType)
    Q_PROPERTY(Qt::MouseButtons mouseButtons READ mouseButtons WRITE setMouseButtons)
    Q_PROPERTY(int key READ key WRITE setKey)
    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers WRITE setModifiers)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(bool autorepeat READ autorepeat WRITE setAutorepeat)

    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QVariantList urls READ urls WRITE setUrls)

    Q_PROPERTY(uint joyProperty READ joyProperty WRITE setJoyProperty) //axis, button

    Q_OBJECT
public:

    //A quite simplified model of input
    enum EventSource {
        None,

        InputMethod,
        DropEvent,

        Mouse,Scroll,Keyboard,

        Gamepad,

        Touch //got nuthin'
    };
    enum JoystickEventType {
        JoyNoneEvent,
        JoyButtonEvent,
        JoyAxisEvent
    };

    Q_ENUMS(EventSource)
    Q_ENUMS(JoystickEventType)

    CoffeeInputEvent(QObject *parent);
    CoffeeInputEvent(QObject *parent, QEvent ev); //QEvent is not available in QtScript apparently
    CoffeeInputEvent(QObject* parent, const QMouseEvent &ev);
    CoffeeInputEvent(QObject* parent, const QWheelEvent &ev);
    CoffeeInputEvent(QObject* parent, const QKeyEvent &ev);

    EventSource type() const;
    JoystickEventType jType() const;

    QPointF pos() const;
    Qt::KeyboardModifiers modifiers() const;
    uint key() const;
    Qt::MouseButtons mouseButtons() const;
    uint joyProperty() const;
    QString text() const;
    bool autorepeat() const;
    QVariantList urls() const;

    QEvent::Type qtype() const;

public slots:
    void accept(); //destroys the object

    void setType(EventSource type);
    void setJType(JoystickEventType jType);

    void setPos(QPointF pos);
    void setModifiers(Qt::KeyboardModifiers modifiers);
    void setKey(uint key);
    void setMouseButtons(Qt::MouseButtons mouseButtons);
    void setJoyProperty(uint joyProperty);
    void setText(QString text);
    void setAutorepeat(bool autorepeat);
    void setUrls(QVariantList urls);

private:
    EventSource m_type = None;
    JoystickEventType m_jType = JoyNoneEvent;
    QPointF m_pos;
    Qt::KeyboardModifiers m_modifiers = Qt::NoModifier;
    uint m_key = Qt::Key_unknown;
    Qt::MouseButtons m_mouseButtons = Qt::NoButton;
    uint m_joyProperty = 0;
    QString m_text;
    bool m_autorepeat;
    QVariantList m_urls;
    QEvent::Type m_qtype;
};

#endif // COFFEEINPUTEVENT_H
