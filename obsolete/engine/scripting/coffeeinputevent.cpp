#include "coffeeinputevent.h"


Coffee::CInput::CIEventParser::CIEventParser(QObject *parent) : QObject(parent)
{
}

void Coffee::CInput::CIEventParser::createEvent(Coffee::CInput::CIEvent::EventType i_type, const void *i_data, uint32_t i_size, void** o_data, uint32_t &o_size)
{
    CIEvent e;
    e.type = i_type;

    *o_data = realloc(*o_data,sizeof(e)+i_size);
    char* bytes = reinterpret_cast<char*>(*o_data);

    memcpy(bytes,&e,sizeof(e));
    memcpy(bytes+sizeof(e),i_data,i_size);
    o_size = sizeof(e)+i_size;
}

void Coffee::CInput::CIEventParser::receiveEvent(void *data, uint32_t size)
{
    if(size==0||!data)
        return;
    char* bytes = reinterpret_cast<char*>(data);
    CIEvent *e = reinterpret_cast<CIEvent*>(bytes);
    size-= sizeof(CIEvent);

    switch(e->type){
    case CIEvent::Mouse:{
        CIMouseEvent* m = reinterpret_cast<CIMouseEvent*>(bytes+sizeof(CIEvent));
        switch(m->type){
        case CIMouseEvent::Press:
        case CIMouseEvent::Release:{
            mouseButton((m->type==CIMouseEvent::Press),m->keyCode,m->modifier);
            break;
        }
        case CIMouseEvent::Move:{
            mouseMove(m->x,m->y);
            break;
        }
        case CIMouseEvent::Enter:
        case CIMouseEvent::Leave:{
            mouseEnter(m->type==CIMouseEvent::Enter);
            break;
        }
        }
        break;
    }
    case CIEvent::Keyboard:{
        CIKeyEvent* k = reinterpret_cast<CIKeyEvent*>(data)+sizeof(CIEvent);
        switch(k->type){
        case CIKeyEvent::Repeated:
        case CIKeyEvent::Press:
        case CIKeyEvent::Release:{
            keyboardKey(k->type-1,k->keyCode,k->modifier);
            break;
        }
        case CIKeyEvent::Text:{
            keyboardEntry(k->keyCode);
            break;
        }
        }

        break;
    }
    }
}
