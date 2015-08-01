#include "coffeepropertycontainer.h"

CoffeePropertyContainer::CoffeePropertyContainer(QObject *parent) : QObject(parent)
{
}

CoffeePropertyContainer::property_takeout CoffeePropertyContainer::getProperty(uint32_t id)
{
    property_takeout d;
    d.size = 0;
    d.data = nullptr;

    for(auto it = m_properties.begin(); it != m_properties.end(); it++)
        if(it->first == id){
            d.size = it->second.size;
            d.data = &m_data.data()[it->second.offset];
        }
    return d;
}

uint32_t CoffeePropertyContainer::addProperty(uint64_t size, void *data)
{
    //TODO : Add function to check for empty space in m_data and move data if necessary
    // For now, we will act as if it will remain contiguous

    m_data.resize(m_data.size()+size);
    memcpy(&m_data.data()[m_data.size()-size],data,size);

    property_container d;
    d.offset = m_data.size()-size;
    d.size = size;
    d.id = getFreeId();

    m_properties.insert(property_pair(d.id,d));

    return d.id;
}

bool CoffeePropertyContainer::setProperty(uint32_t id, uint64_t size, void *data)
{
    for(auto it = m_properties.begin();it != m_properties.end();it++){
        if(it->first == id){
            property_container d = it->second;
            if(d.size!=size)
                return false;
            memcpy(&m_data.data()[d.offset],data,size);
            return true;
        }
    }
    return false;
}

bool CoffeePropertyContainer::removeProperty(uint32_t id)
{
    for(auto it = m_properties.begin();it != m_properties.end();it++)
        if(it->first == id){
            m_properties.erase(it);
            return true;
        }
    return false;
}

bool CoffeePropertyContainer::hasProperty(uint32_t id)
{
    for(auto it = m_properties.begin();it != m_properties.end(); it++)
        if(it->first == id)
            return true;
    return false;
}

uint32_t CoffeePropertyContainer::getFreeId()
{
    //Quite naive, does not find IDs unless it is greater than all others.

    uint32_t greatest = 0;
    for(auto it = m_properties.begin();it!=m_properties.end();it++)
        if(it->first > greatest)
            greatest = it->first;
    return greatest;
}
