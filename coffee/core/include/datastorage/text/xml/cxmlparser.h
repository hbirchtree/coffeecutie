#ifndef COFFEE_CORE_DATASTORAGE_TEXT_XML_H
#define COFFEE_CORE_DATASTORAGE_TEXT_XML_H

#include <coffee/core/CBase>
#include <tinyxml2/tinyxml2.h>

namespace Coffee{
namespace CDataStorage{
namespace CTextStorage{
namespace XML{

using namespace tinyxml2;

struct TinyXML2
{
    using Document = XMLDocument;
    using Element = XMLElement;

    inline C_FORCE_INLINE static const XMLDocument* XMLRead(
	    const CResources::CResource& source)
    {
	XMLDocument* doc = new XMLDocument;
	doc->Parse((cstring)source.data,source.size);
	return doc;
    }

};

}
}
}

using XML = CDataStorage::CTextStorage::XML::TinyXML2;

}

#endif
