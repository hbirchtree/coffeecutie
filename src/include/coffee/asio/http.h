#pragma once

#include <coffee/core/string_casting.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <iterator>

namespace http {

using namespace ::Coffee::Primitives;

using ::Coffee::cast_pod;
using ::Coffee::cast_string;

using string       = std::string;
using plain_string = const string::value_type*;

namespace util {
namespace strings {
namespace detail {
inline bool not_space(string::value_type c)
{
    return !std::isspace(c);
}
} // namespace detail

inline bool iequals(string const& v1, string const& v2)
{
    auto it1 = v1.begin(), it2 = v2.begin();

    while(it1 != v1.end() && it2 != v2.end())
    {
        if(std::tolower(*it1) != std::tolower(*it2))
            return false;

        ++it1;
        ++it2;
    }
    return true;
}
inline string trim(string const& value_)
{
    string value = value_;

    value.erase(
        value.begin(),
        std::find_if(value.begin(), value.end(), detail::not_space));

    return value;
}
} // namespace strings
} // namespace util

enum class version : u8
{
    v10 = 10,
    v11 = 11,
    v20 = 20,
};

enum class header_field : u8
{
    none,

    accept,
    accept_charset,
    accept_encoding,
    accept_language,
    connection,
    content_encoding,
    content_language,
    content_length,
    content_location,
    content_type,
    expect,
    host,
    location,
    server,
    user_agent,
};

enum class content_type : u8
{
    any,

    /* text types */
    text,
    json,
    xml,

    /* binary types */
    octet_stream,
    audio,
    image,
    video,
};

enum class method : u8
{
    delet,
    get,
    head,
    options,
    patch,
    post,
    put,
    update,
};

enum class connection_policy : u8
{
    close,
    keepalive,
};

enum class response_class : u8
{
    unknown,

    info,
    success,
    redirect,
    client_error,
    server_error,
};

namespace status {
enum status_value
{
    continue_ = 100,

    ok              = 200,
    created         = 201,
    accepted        = 202,
    no_content      = 204,
    reset_content   = 205,
    partial_content = 206,

    moved_permanently  = 301,
    found              = 302,
    temporary_redirect = 307,

    bad_request         = 400,
    unauthorized        = 401,
    forbidden           = 403,
    not_found           = 404,
    method_invalid      = 405,
    not_acceptable      = 406,
    request_timeout     = 408,
    conflict            = 409,
    gone                = 410,
    length_required     = 411,
    precondition_failed = 412,
    too_large           = 413,
    uri_too_long        = 414,
    unsupported_media   = 415,
    bad_request_range   = 416,
    expectation_failed  = 417,

    server_error        = 500,
    not_implemented     = 501,
    bad_gateway         = 502,
    service_unavailable = 503,
    gateway_timeout     = 504,
    version_mismatch    = 505,
};
}

static constexpr plain_string line_separator    = "\r\n";
static constexpr plain_string header_terminator = "\r\n\r\n";

struct header_t
{
    header_t() :
        resource("/"), code(0), version(version::v10), method(method::get)
    {
    }

    string message;  /*!< method on request, message on response */
    string resource; /*!< request-only */
    std::map<header_field, string> standard_fields;
    std::map<string, string>       fields;

    u16     code; /*!< response-only */
    version version;
    method  method;
};

using payload_t = std::vector<char>;

struct request_t
{
    string    host;
    u16       port;
    header_t  header;
    payload_t payload;
};

struct response_t
{
    u16       code; /*!< response code for constructed response */
    header_t  header;
    payload_t payload; /*!< payload for constructed response */
};

namespace header {
namespace versioning {
inline void ensure(header_t& header, version v)
{
    auto existing = static_cast<u32>(header.version);
    auto ensure   = static_cast<u32>(v);

    if(existing < ensure)
        header.version = v;
}
} // namespace versioning

namespace to_string {
inline plain_string version(version v)
{
    switch(v)
    {
    case version::v10:
        return "HTTP/1.0";
    case version::v11:
        return "HTTP/1.1";
    case version::v20:
        return "HTTP/2.0";
    }
}

inline plain_string field(header_field f)
{
    switch(f)
    {
    case header_field::accept:
        return "Accept";
    case header_field::accept_charset:
        return "Accept-Charset";
    case header_field::accept_encoding:
        return "Accept-Encoding";
    case header_field::accept_language:
        return "Accept-Language";
    case header_field::connection:
        return "Connection";
    case header_field::content_encoding:
        return "Content-Encoding";
    case header_field::content_language:
        return "Content-Language";
    case header_field::content_length:
        return "Content-Length";
    case header_field::content_location:
        return "Content-Location";
    case header_field::content_type:
        return "Content-Type";
    case header_field::expect:
        return "Expect";
    case header_field::host:
        return "Host";
    case header_field::location:
        return "Location";
    case header_field::server:
        return "Server";
    case header_field::user_agent:
        return "User-Agent";

    case header_field::none:
        return "";
    }
}

inline plain_string content_type(http::content_type type)
{
    switch(type)
    {
    case http::content_type::any:
        return "*/*";

    case http::content_type::text:
        return "text/plain";
    case http::content_type::json:
        return "application/json";
    case http::content_type::xml:
        return "application/xml";

    case http::content_type::octet_stream:
        return "application/octet-stream";
    case http::content_type::audio:
        return "audio/*";
    case http::content_type::image:
        return "image/*";
    case http::content_type::video:
        return "video/*";
    }
}

inline plain_string method(http::method meth)
{
    switch(meth)
    {
    case http::method::delet:
        return "DELETE";
    case http::method::get:
        return "GET";
    case http::method::head:
        return "HEAD";
    case http::method::options:
        return "OPTIONS";
    case http::method::patch:
        return "PATCH";
    case http::method::post:
        return "POST";
    case http::method::put:
        return "PUT";
    case http::method::update:
        return "UPDATE";
    }
}

inline plain_string connection_policy(http::connection_policy pol)
{
    switch(pol)
    {
    case http::connection_policy::close:
        return "close";
    case http::connection_policy::keepalive:
        return "keep-alive";
    }
}
} // namespace to_string

namespace from_string {
inline version version(string const& v)
{
    if(v.substr(0, 5) != "HTTP/")
        return version::v10;

    auto ver_sub = v.substr(5, 3);

    if(ver_sub == "1.0")
        return version::v10;
    if(ver_sub == "1.1")
        return version::v11;
    if(ver_sub == "2.0")
        return version::v20;

    return version::v10;
}

inline header_field field(string const& f)
{
    if(util::strings::iequals(f, "Accept"))
        return header_field::accept;
    if(util::strings::iequals(f, "Accept-Charset"))
        return header_field::accept_charset;
    if(util::strings::iequals(f, "Accept-Encoding"))
        return header_field::accept_encoding;
    if(util::strings::iequals(f, "Accept-Language"))
        return header_field::accept_language;
    if(util::strings::iequals(f, "Connection"))
        return header_field::connection;
    if(util::strings::iequals(f, "Content-Encoding"))
        return header_field::content_encoding;
    if(util::strings::iequals(f, "Content-Language"))
        return header_field::content_language;
    if(util::strings::iequals(f, "Content-Length"))
        return header_field::content_length;
    if(util::strings::iequals(f, "Content-Location"))
        return header_field::content_location;
    if(util::strings::iequals(f, "Content-Type"))
        return header_field::content_type;
    if(util::strings::iequals(f, "Expect"))
        return header_field::expect;
    if(util::strings::iequals(f, "Host"))
        return header_field::host;
    if(util::strings::iequals(f, "Location"))
        return header_field::location;
    if(util::strings::iequals(f, "Server"))
        return header_field::server;
    if(util::strings::iequals(f, "User-Agent"))
        return header_field::user_agent;

    return header_field::none;
}

inline http::method method(string const& v)
{
    if(v == "DELETE")
        return http::method::delet;
    if(v == "GET")
        return http::method::get;
    if(v == "HEAD")
        return http::method::head;
    if(v == "OPTIONS")
        return http::method::options;
    if(v == "PATCH")
        return http::method::patch;
    if(v == "POST")
        return http::method::post;
    if(v == "PUT")
        return http::method::put;
    if(v == "UPDATE")
        return http::method::update;

    return http::method::get;
}
} // namespace from_string

namespace serialize {
/*!
 * \brief Outputs:
 * `[method] [resource] HTTP/[version]`
 * \param header
 * \return
 */
inline string request_line(header_t const& header)
{
    return to_string::method(header.method) + string(" ") + header.resource +
           " " + to_string::version(header.version) + line_separator;
}

/*!
 * \brief Outputs:
 * `HTTP/[version] [status code] [message]`
 * \param header
 * \return
 */
inline string response_line(header_t const& header)
{
    return to_string::version(header.version) + string(" ") +
           cast_pod(header.code) + " " + header.message + line_separator;
}

inline string field(string const& key, string const& value)
{
    return key + ": " + value + line_separator;
}

inline string fields(header_t const& header)
{
    string out;

    for(auto const& field : header.standard_fields)
        out += serialize::field(to_string::field(field.first), field.second);

    for(auto const& field : header.fields)
        out += serialize::field(field.first, field.second);

    return out;
}

inline string request(header_t const& header)
{
    string out;

    out += serialize::request_line(header);
    out += serialize::fields(header);
    out += line_separator;

    return out;
}

inline string response(header_t const& header)
{
    string out;

    out += serialize::response_line(header);
    out += serialize::fields(header);
    out += line_separator;

    return out;
}
} // namespace serialize

namespace parse {
inline string read_value(std::istream& h)
{
    string out;
    h >> out;
    return out;
}

inline string read_line(std::istream& h)
{
    string out;
    std::getline(h, out);
    out = out.substr(0, out.find("\r"));
    return out;
}

template<typename Rep>
inline Rep read_int(std::istream& h)
{
    auto value = read_value(h);
    return cast_string<Rep>(value);
}

inline header_t& response_line(header_t& h, std::istream& s)
{
    h.version = header::from_string::version(read_value(s));
    h.code    = read_int<u16>(s);
    h.message = util::strings::trim(read_line(s));

    return h;
}

inline header_t& request_line(header_t& h, std::istream& s)
{
    h.method   = from_string::method(read_value(s));
    h.resource = read_value(s);
    h.code     = read_int<u16>(s);
    h.message  = read_line(s);

    return h;
}

inline std::pair<string, string> field(string const& line)
{
    auto split = line.find(":");
    auto len   = line.size();

    auto key   = line.substr(0, split);
    auto value = line.substr(split + 1, len - split - 2);

    return {key, util::strings::trim(std::move(value))};
}

using optional_field =
    std::pair<std::pair<header_field, string>, std::pair<string, string>>;

inline optional_field field_classify(string const& line)
{
    auto values = field(line);

    auto key = from_string::field(values.first);

    return {{key, values.second}, values};
}

struct field_iterator
{
    field_iterator(string const& value, string::value_type sep) :
        m_value(" " + value), m_pos(0), m_separator(sep)
    {
    }

    field_iterator& operator++()
    {
        if(m_pos == string::npos)
            Throw(std::out_of_range("no more values"));

        m_pos = m_value.find(m_separator, m_pos + 1);

        return *this;
    }

    string operator*()
    {
        auto next_value = m_value.find(m_separator, m_pos + 1);

        auto value = m_value.substr(m_pos + 1, next_value - 1);

        return util::strings::trim(value);
    }

  private:
    string             m_value;
    szptr              m_pos;
    string::value_type m_separator;
};
} // namespace parse

namespace transform {
inline header_t& create_continue(header_t& origin)
{
    versioning::ensure(origin, version::v11);

    origin.standard_fields[header_field::expect] = "100-Continue";

    return origin;
}

inline request_t& create_request(request_t& origin)
{
    auto& standard_headers = origin.header.standard_fields;

    auto host = origin.host /* + ":" + cast_pod(origin.port)*/;
    standard_headers[header_field::host] = host;

    if(origin.payload.size())
    {
        auto content_length = cast_pod(origin.payload.size());
        standard_headers[header_field::content_length] = content_length;
    }

    auto cont_type = standard_headers.find(header_field::content_type);
    if(cont_type == standard_headers.end() && origin.payload.size())
    {
        standard_headers[header_field::content_type] =
            to_string::content_type(content_type::octet_stream);
    }

    return origin;
}
} // namespace transform

inline response_class classify_status(u16 code)
{
    if(code < 100)
        return response_class::unknown;

    if(code < 199) // 1xx
        return response_class::info;
    if(code < 299) // 2xx
        return response_class::success;
    if(code < 399) // 3xx
        return response_class::redirect;
    if(code < 499) // 4xx
        return response_class::client_error;
    if(code < 599) // 5xx
        return response_class::server_error;

    return response_class::unknown;
}

} // namespace header

namespace resource {
inline string& add_query(string& resource, string query, string value)
{
    auto queryPos = resource.find('?');

    auto separator = '?';

    if(queryPos != string::npos)
        separator = '&';

    resource += separator + query + '=' + value;

    return resource;
}

struct path_iterator
{
    path_iterator(string const& resource) :
        m_pos(0), m_resource(resource), m_query_start(m_resource.find('?'))
    {
        if(m_resource.substr(0, 1) != "/")
            m_resource = "/" + m_resource;
    }

    inline path_iterator& operator++()
    {
        if(invalid())
            Throw(std::out_of_range("no path components"));

        m_pos = m_resource.find('/', m_pos + 1);

        return *this;
    }

    inline string operator*()
    {
        auto end = m_query_start == string::npos ? string::npos
                                                 : m_query_start - m_pos + 1;
        auto next_comp = m_resource.find('/', m_pos + 1) - m_pos;

        return m_resource.substr(m_pos + 1, std::min(next_comp, end) - 1);
    }

    inline bool invalid() const
    {
        return m_pos == string::npos;
    }

  private:
    szptr  m_pos;
    string m_resource;
    szptr  m_query_start;
};

struct query_iterator
{
    query_iterator(string const& resource) :
        m_pos(string::npos), m_resource(resource)
    {
        m_pos = m_resource.find("?");
    }

    query_iterator& operator++()
    {
        if(invalid())
            Throw(std::out_of_range("no parameters"));

        m_pos = m_resource.find('&', m_pos + 1);

        return *this;
    }

    std::pair<string, string> operator*()
    {
        auto end        = m_resource.size() - m_pos; /* guaranteed end */
        auto next_param = m_resource.find('&', m_pos + 1) - m_pos;

        auto full = m_resource.substr(m_pos + 1, std::min(next_param, end) - 1);
        auto split = full.find('=');

        if(split == string::npos)
            return {};

        return {full.substr(0, split), full.substr(split + 1)};
    }

  private:
    inline bool invalid() const
    {
        return m_pos == string::npos;
    }

    szptr  m_pos;
    string m_resource;
};
} // namespace resource

namespace validate {
namespace util {
template<typename MapType>
inline bool has_key(MapType const& map, typename MapType::key_type k)
{
    return map.find(k) != map.end();
}
} // namespace util

enum class result
{
    valid,

    no_host,
    no_accept,
    semantic_error,
};

inline result request(request_t const& rq)
{
    auto const& st_fields = rq.header.standard_fields;

    if(!util::has_key(st_fields, header_field::host))
        return result::no_host;

    if(!util::has_key(st_fields, header_field::accept))
        return result::no_accept;

    if(rq.header.method == method::get && rq.payload.size() > 0)
        return result::semantic_error;

    return result::valid;
}
} // namespace validate

namespace stream {
inline header_t read_response(std::istream& h)
{
    header_t out = {};

    header::parse::response_line(out, h);

    string line_store;
    while(std::getline(h, line_store) && line_store != "\r")
    {
        auto classified = header::parse::field_classify(line_store);

        if(classified.first.first != header_field::none)
            out.standard_fields.insert(classified.first);
        else
            out.fields.insert(classified.second);
    }

    return out;
}

inline payload_t read_payload(
    std::istream& p, size_t len, std::function<void(size_t)> progress = nullptr)
{
    payload_t payload;
    payload.resize(len);

    auto      payload_ptr = payload.data();
    ::off64_t buf_size    = static_cast<::off64_t>(payload.size());
    ::off64_t offset      = 0;

    while(p.read(&payload_ptr[offset], buf_size - offset) && offset < buf_size)
    {
        if(progress)
            progress(payload.size());
        offset += p.gcount();
    }

    return payload;
}

} // namespace stream

} // namespace http
