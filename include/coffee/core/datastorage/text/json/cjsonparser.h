#ifndef COFFEE_CORE_DATASTORAGE_TEXT_JSON_H
#define COFFEE_CORE_DATASTORAGE_TEXT_JSON_H

#include <coffee/core/CBase>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

namespace Coffee{
namespace CDataStorage{
namespace CTextStorage{
namespace RJSON{
using namespace rapidjson;

struct RapidJson
{
    using Document = rapidjson::Document;

    inline C_FORCE_INLINE static rapidjson::Document Read(cstring data)
    {
	Document doc;
	doc.Parse(data);
	return doc;
    }
};

}
}
}

using JSON = CDataStorage::CTextStorage::RJSON::RapidJson;

}

#endif
