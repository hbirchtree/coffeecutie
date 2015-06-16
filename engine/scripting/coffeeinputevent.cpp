#include "coffeeinputevent.h"

CoffeeInputEvent::CoffeeInputEvent(QObject *parent) : QObject(parent)
{

}

CoffeeInputEvent::CoffeeInputEvent(QObject *parent, const QEvent &ev) : CoffeeInputEvent(parent)
{
    switch(ev.type()){
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:{
        //MouseEvent
        break;
    }
    case QEvent::InputMethod:{
        //Typing, handled specifically, but is actually a KeyEvent
        break;
    }
    case QEvent::KeyRelease:
    case QEvent::KeyPress:{
        //KeyEvent
        break;
    }
    case QEvent::FocusIn:
    case QEvent::FocusOut:{
        //FocusEvent
        break;
    }
    case QEvent::Wheel:{
        //WheelEvent
        break;
    }
    case QEvent::Drop:{
        //DropEvent
        break;
    }
    default:break;
    }
    m_data.append("Hello");
}

CoffeeInputEvent::EventSource CoffeeInputEvent::type() const
{
    return m_type;
}

QVariantList CoffeeInputEvent::data() const
{
    return m_data;
}

void CoffeeInputEvent::setType(CoffeeInputEvent::EventSource type)
{
    m_type = type;
}

void CoffeeInputEvent::setData(QVariantList data)
{
    m_data = data;
}

void CoffeeInputEvent::accept()
{
    this->deleteLater();
}
