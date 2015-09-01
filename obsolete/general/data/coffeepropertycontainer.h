#ifndef COFFEEPROPERTYCONTAINER_H
#define COFFEEPROPERTYCONTAINER_H

#include <QObject>
#include <vector>

class CoffeePropertyContainer : public QObject
{
    Q_OBJECT

protected:
    CoffeePropertyContainer(QObject* parent);

    struct property_takeout
    {
        uint64_t size;
        void* data;
    };

    property_takeout getProperty(uint32_t id);
    uint32_t addProperty(uint64_t size, void* data); //returns property flag for the chunk
    bool setProperty(uint32_t id, uint64_t size, void* data); //size must match for this to be successful
    bool removeProperty(uint32_t id);

    bool hasProperty(uint32_t id); //id increases with amount of properties

private:
    uint32_t getFreeId();

    struct property_container
    {
        uint64_t offset;
        uint64_t size;
        uint32_t id;
    };

    typedef std::pair<uint32_t,property_container> property_pair;

    std::map<uint32_t,property_container> m_properties;
    std::vector<uint8_t> m_data;
};

#endif // COFFEEPROPERTYCONTAINER_H
