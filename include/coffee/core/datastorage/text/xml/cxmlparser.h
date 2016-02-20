#ifndef COFFEE_CORE_DATASTORAGE_TEXT_XML_H
#define COFFEE_CORE_DATASTORAGE_TEXT_XML_H

#include "../../../types/composite_types.h"
#include "../../../base/cfiles.h"
#include <tinyxml2/tinyxml2.h>

namespace Coffee{
namespace CDataStorage{
namespace CTextStorage{
namespace XML{

using namespace tinyxml2;

struct TinyXML2Impl
{
    using Document = tinyxml2::XMLDocument;
    using Element = tinyxml2::XMLElement;

    STATICINLINE Document* XMLRead(
	    const CResources::CResource& source)
    {
		Document* doc = new Document;
		doc->Parse((cstring)source.data,source.size);
		return doc;
    }

};

}
}
}

using XML = CDataStorage::CTextStorage::XML::TinyXML2Impl;

}

#endif
