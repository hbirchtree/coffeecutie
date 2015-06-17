#include "coffeeinputevent.h"

#include <QMimeData>

CoffeeInputEvent::CoffeeInputEvent(QObject *parent) : QObject(parent)
{

}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, const QEvent &ev) : CoffeeInputEvent(parent)
{
//    real_event = ev;

    switch(ev.type()){
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:{
        m_type = Mouse;
        QMouseEvent *e = (QMouseEvent*)(&ev);
        setMouseButtons(e->button());
        setModifiers(e->modifiers());
        setPos(e->pos());
        break;
    }
    case QEvent::InputMethod:{
        m_type = InputMethod;
        QKeyEvent *e = (QKeyEvent*)(&ev);
        setText(e->text());
        break;
    }
    case QEvent::KeyRelease:
    case QEvent::KeyPress:{
        m_type = Keyboard;
        QKeyEvent *e = (QKeyEvent*)(&ev);
        setKey(e->key());
        setModifiers(e->modifiers());
        setAutorepeat(e->isAutoRepeat());
        break;
    }
    case QEvent::FocusIn:
    case QEvent::FocusOut:{
        break;
    }
    case QEvent::Wheel:{
        //WheelEvent
        break;
    }
    case QEvent::Drop:{
        m_type = DropEvent;
        QDropEvent *e = (QDropEvent*)(&ev);
        setModifiers(e->keyboardModifiers());
        setMouseButtons(e->mouseButtons());
        const QMimeData* d = e->mimeData();
        if(e->mimeData()->hasText()){
            qDebug() << e->mimeData()->text();
            setText(e->mimeData()->text());
        }
        if(e->mimeData()->hasUrls()){
            QVariantList l;
            for(QUrl u : e->mimeData()->urls())
                l.append(u);
            setUrls(l);
        }
        break;
    }
    default:break;
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
