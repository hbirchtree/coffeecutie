#ifndef COFFEE_CORE_DATASTORAGE_TEXT_JSON_H
#define COFFEE_CORE_DATASTORAGE_TEXT_JSON_H

#include <coffee/core/CBase>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace Coffee{
namespace DataStorage{
namespace TextStorage{
namespace RJSON{

using namespace rapidjson;

struct RapidJson
{
    using Document = rapidjson::Document;
    using Value = rapidjson::Value;

    using WriteBuf = rapidjson::StringBuffer;
    using Writer = rapidjson::Writer<rapidjson::StringBuffer>;

    STATICINLINE Document Read(cstring data)
    {
	Document doc;
	doc.Parse(data);
	return doc;
    }
    STATICINLINE CString Serialize(Document const& doc)
    {
        WriteBuf buf;
        buf.Clear();

        Writer writer(buf);
        doc.Accept(writer);
        return buf.GetString();
    }
};

}
}
}

using JSON = DataStorage::TextStorage::RJSON::RapidJson;

}

#endif
