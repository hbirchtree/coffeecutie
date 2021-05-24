#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

#include <tinyxml2.h>

namespace xml {

using namespace tinyxml2;

using Node      = tinyxml2::XMLNode;
using Attribute = tinyxml2::XMLAttribute;
using Document  = tinyxml2::XMLDocument;
using Element   = tinyxml2::XMLElement;

STATICINLINE Document* Read(semantic::Bytes const& data)
{
    Document* doc = new Document;
    doc->Parse(data.as<char>().data, data.size);
    return doc;
}

} // namespace xml
