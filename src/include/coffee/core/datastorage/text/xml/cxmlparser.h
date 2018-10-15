#pragma once

#include "../../../base/files/cfiles.h"
#include "../../../types/tdef/integertypes.h"

#include <tinyxml2.h>

namespace Coffee {
namespace DataStorage {
namespace TextStorage {
namespace XML {

using namespace tinyxml2;

struct TinyXML2Impl
{
    using Node      = tinyxml2::XMLNode;
    using Attribute = tinyxml2::XMLAttribute;
    using Document  = tinyxml2::XMLDocument;
    using Element   = tinyxml2::XMLElement;

    STATICINLINE Document* Read(Bytes const& data)
    {
        Document* doc = new Document;
        doc->Parse(C_RCAST<cstring>(data.data), data.size);
        return doc;
    }
};

} // namespace XML
} // namespace TextStorage
} // namespace DataStorage

using XML = DataStorage::TextStorage::XML::TinyXML2Impl;

} // namespace Coffee
