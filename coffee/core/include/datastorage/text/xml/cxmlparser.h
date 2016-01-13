#ifndef COFFEE_CORE_DATASTORAGE_TEXT_XML_H
#define COFFEE_CORE_DATASTORAGE_TEXT_XML_H

#include <coffee/core/CBase>
#include <tinyxml2/tinyxml2.h>

namespace Coffee{
namespace CDataStorage{
namespace CTextStorage{

using namespace tinyxml2;

const XMLDocument* coffee_xml_read(const CResources::CResource& source)
{
    XMLDocument* doc = new XMLDocument;
    doc->Parse((cstring)source.data,source.size);
    return doc;
}

}
}
}

#endif
