#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

#include <string>
#include <string_view>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>

namespace json {

using namespace rapidjson;

using Document = rapidjson::Document;
using Value    = rapidjson::Value;

using WriteBuf = rapidjson::StringBuffer;
using Writer   = rapidjson::Writer<rapidjson::StringBuffer>;

struct Object : Value
{
    Object()
    {
        Value::SetObject();
    }
};

struct Array : Value
{
    Array()
    {
        Value::SetArray();
    }
};

STATICINLINE Document Read(semantic::BytesConst const& data)
{
    Document doc;
    doc.Parse(data.as<const char>().data);
    return doc;
}
STATICINLINE stl_types::CString Serialize(Document const& doc)
{
    WriteBuf buf;
    buf.Clear();

    Writer writer(buf);
    doc.Accept(writer);
    return buf.GetString();
}

namespace detail {

template<typename T>
FORCEDINLINE json::Value wrap_value(T value, Document::AllocatorType&)
{
    return json::Value(value);
}

template<>
FORCEDINLINE json::Value wrap_value<std::string>(
    std::string value, Document::AllocatorType& alloc)
{
    return json::Value(
        value.c_str(), C_FCAST<libc_types::u32>(value.size()), alloc);
}

template<>
FORCEDINLINE json::Value wrap_value<const char*>(
    const char* value, Document::AllocatorType& alloc)
{
    return json::Value(value, alloc);
}

template<typename T>
FORCEDINLINE void set_member(
    Object& obj, Value&& field, T&& value, Document::AllocatorType& alloc)
{
    obj.AddMember(field, detail::wrap_value(value, alloc), alloc);
}

template<>
FORCEDINLINE void set_member<Value>(
    Object& obj, Value&& field, Value&& value, Document::AllocatorType& alloc)
{
    obj.AddMember(field, std::move(value), alloc);
}
template<>
FORCEDINLINE void set_member<Object>(
    Object& obj, Value&& field, Object&& value, Document::AllocatorType& alloc)
{
    obj.AddMember(field, std::move(value), alloc);
}
template<>
FORCEDINLINE void set_member<Array>(
    Object& obj, Value&& field, Array&& value, Document::AllocatorType& alloc)
{
    obj.AddMember(field, std::move(value), alloc);
}

} // namespace detail

struct ObjectBuilder
{
    ObjectBuilder(Document::AllocatorType& allocator) : m_alloc(allocator)
    {
    }

    template<typename T>
    ObjectBuilder& put(std::string const& field, T value)
    {
        detail::set_member<T>(
            m_object,
            detail::wrap_value(field, m_alloc),
            std::move(value),
            m_alloc);
        return *this;
    }

    Object&& eject()
    {
        return std::move(m_object);
    }

    Document::AllocatorType& allocator()
    {
        return m_alloc;
    }

  private:
    Object                   m_object;
    Document::AllocatorType& m_alloc;
};

struct ArrayBuilder
{
    ArrayBuilder(Document::AllocatorType& allocator) : m_alloc(allocator)
    {
    }

    template<typename T>
    ArrayBuilder& push_back(T value)
    {
        m_array.PushBack(detail::wrap_value(value, m_alloc), m_alloc);
        return *this;
    }

    Array&& eject()
    {
        return std::move(m_array);
    }

  private:
    Array                    m_array;
    Document::AllocatorType& m_alloc;
};

} // namespace json
