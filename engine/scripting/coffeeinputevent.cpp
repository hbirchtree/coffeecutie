#include "coffeeinputevent.h"

#include <QMimeData>

CoffeeInputEvent::CoffeeInputEvent(QObject *parent) : QObject(parent)
{

}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, QEvent ev) : CoffeeInputEvent(parent)
{
    m_qtype = ev.type();
    switch(ev.type()){
    case QEvent::Drop:{
//        m_type = DropEvent;
//        QDropEvent *e = (QDropEvent*)&ev;
//        setModifiers(e->keyboardModifiers());
//        setMouseButtons(e->mouseButtons());
//        const QMimeData* d = e->mimeData();
//        if(e->mimeData()->hasText()){
//            qDebug() << e->mimeData()->text();
//            setText(e->mimeData()->text());
//        }
//        if(e->mimeData()->hasUrls()){
//            QVariantList l;
//            for(QUrl u : e->mimeData()->urls())
//                l.append(u);
//            setUrls(l);
//        }
        break;
    }
    default:break;
    }
}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, const QMouseEvent &ev) : CoffeeInputEvent(parent)
{
    m_type = Mouse;
    m_qtype = ev.type();
    setAutorepeat(ev.type()==QEvent::MouseButtonRelease);
    setMouseButtons(ev.button());
    setModifiers(ev.modifiers());
    setPos(ev.pos());
}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, const QWheelEvent &ev) : CoffeeInputEvent(parent)
{
    m_type = Scroll;
    m_qtype = ev.type();
    setPos(ev.angleDelta());
}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, const QKeyEvent &ev) : CoffeeInputEvent(parent)
{
    m_qtype = ev.type();
    switch(ev.type()){
    case QEvent::InputMethod:{
        m_type = InputMethod;
        setText(ev.text());
        break;
    }
    default:{
        m_type = Keyboard;
        setKey(ev.key());
        setModifiers(ev.modifiers());
        setAutorepeat(ev.isAutoRepeat());
        setText(ev.text());
    }
    }
}

CoffeeInputEvent::EventSource CoffeeInputEvent::type() const
{
    return m_type;
}

CoffeeInputEvent::JoystickEventType CoffeeInputEvent::jType() const
{
    return m_jType;
}

QPointF CoffeeInputEvent::pos() const
{
    return m_pos;
}

Qt::KeyboardModifiers CoffeeInputEvent::modifiers() const
{
    return m_modifiers;
}

uint CoffeeInputEvent::key() const
{
    return m_key;
}

Qt::MouseButtons CoffeeInputEvent::mouseButtons() const
{
    return m_mouseButtons;
}

uint CoffeeInputEvent::joyProperty() const
{
    return m_joyProperty;
}

QString CoffeeInputEvent::text() const
{
    return m_text;
}

bool CoffeeInputEvent::autorepeat() const
{
    return m_autorepeat;
}

QVariantList CoffeeInputEvent::urls() const
{
    return m_urls;
}

QEvent::Type CoffeeInputEvent::qtype() const
{
    return m_qtype;
}

void CoffeeInputEvent::setType(CoffeeInputEvent::EventSource type)
{
    m_type = type;
}

void CoffeeInputEvent::accept()
{
    this->deleteLater();
}

void CoffeeInputEvent::setPos(QPointF pos)
{
    m_pos = pos;
}

void CoffeeInputEvent::setModifiers(Qt::KeyboardModifiers modifiers)
{
    m_modifiers = modifiers;
}

void CoffeeInputEvent::setKey(uint key)
{
    m_key = key;
}

void CoffeeInputEvent::setMouseButtons(Qt::MouseButtons mouseButtons)
{
    m_mouseButtons = mouseButtons;
}

void CoffeeInputEvent::setJoyProperty(uint joyProperty)
{
    m_joyProperty = joyProperty;
}

void CoffeeInputEvent::setText(QString text)
{
    m_text = text;
}

void CoffeeInputEvent::setAutorepeat(bool autorepeat)
{
    m_autorepeat = autorepeat;
}

void CoffeeInputEvent::setUrls(QVariantList urls)
{
    m_urls = urls;
}

void CoffeeInputEvent::setJType(CoffeeInputEvent::JoystickEventType jType)
{
    m_jType = jType;
}
