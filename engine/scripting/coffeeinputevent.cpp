#include "coffeeinputevent.h"


Coffee::CInput::CIEventParser::CIEventParser(QObject *parent) : QObject(parent)
{
}

void Coffee::CInput::CIEventParser::createEvent(Coffee::CInput::CIEvent::EventType i_type, const void *i_data, uint32_t i_size, void *o_data, uint32_t &o_size)
{
    CIEvent e;
    e.type = i_type;

    o_data = realloc(o_data,sizeof(e)+i_size);

    memcpy(o_data,&e,sizeof(e));
    memcpy(o_data,i_data,i_size);
    o_size = sizeof(e)+i_size;
}

void Coffee::CInput::CIEventParser::receiveEvent(void *data, uint16_t size)
{

}
