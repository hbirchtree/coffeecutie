#ifndef COFFEE_CORE_DATASTORAGE_TEXT_XML_H
#define COFFEE_CORE_DATASTORAGE_TEXT_XML_H

#include "../../../types/tdef/integertypes.h"
#include "../../../base/files/cfiles.h"
#include <tinyxml2/tinyxml2.h>

namespace Coffee{
namespace DataStorage{
namespace TextStorage{
namespace XML{

using namespace tinyxml2;

struct TinyXML2Impl
{
    using Node = tinyxml2::XMLNode;
    using Attribute = tinyxml2::XMLAttribute;
    using Document = tinyxml2::XMLDocument;
    using Element = tinyxml2::XMLElement;

    STATICINLINE Document* XMLRead(
	    const BytesConst& data)
    {
	Document* doc = new Document;
	doc->Parse((cstring)data.data,data.size);
	return doc;
    }

    STATICINLINE Document* XMLRead(
            const CResources::Resource& source)
    {
		return XMLRead(CResources::FileGetDescriptor(source));
    }

};

}
}
}

using XML = DataStorage::TextStorage::XML::TinyXML2Impl;

}

#endif
