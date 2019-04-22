#pragma once

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>

namespace Coffee {
namespace DataStorage {
namespace TextStorage {
namespace RJSON {

using namespace rapidjson;

struct RapidJson
{
    using Document = rapidjson::Document;
    using Value    = rapidjson::Value;

    using WriteBuf = rapidjson::StringBuffer;
    using Writer   = rapidjson::Writer<rapidjson::StringBuffer>;

    struct Object : Value
    {
        Object() {
            Value::SetObject();
        }
    };

    struct Array : Value
    {
        Array() {
            Value::SetArray();
        }
    };

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

} // namespace RJSON
} // namespace TextStorage
} // namespace DataStorage

using JSON = DataStorage::TextStorage::RJSON::RapidJson;

} // namespace Coffee
