#pragma once

#include <iterator>
#include <sstream>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>

#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_GEKKO)
#define COFFEE_HTTP_MULTIPART
#endif

namespace http {

using namespace ::libc_types;

using ::stl_types::cast_pod;
using ::stl_types::cast_string;

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
    if(v1.size() != v2.size())
        return false;

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

enum class version_t : u8
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
    content_disposition,
    content_encoding,
    content_language,
    content_length,
    content_location,
    content_type,
    date,
    expect,
    host,
    location,
    server,
    user_agent,
};

enum class content_type : u8
{
    any,
    multipart_form,

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

enum class method_t : u8
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

struct header_traits
{
    using string_type  = string;
    using value_type   = string::value_type;
    using plain_string = const value_type*;

    static constexpr plain_string header_separator  = http::line_separator;
    static constexpr plain_string header_terminator = http::header_terminator;

    static constexpr plain_string header_end = "\r";
};

struct header_t
{
    header_t() :
        resource("/"), code(0), version(version_t::v10), method(method_t::get)
    {
    }

    string message;  /*!< method on request, message on response */
    string resource; /*!< request-only */
    std::map<header_field, string> standard_fields;
    std::map<string, string>       fields;

    u16       code; /*!< response-only */
    version_t version;
    method_t  method;

    u32 padding;
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
inline void ensure(header_t& header, version_t v)
{
    auto existing = static_cast<u32>(header.version);
    auto ensure   = static_cast<u32>(v);

    if(existing < ensure)
        header.version = v;
}
} // namespace versioning

namespace to_string {
inline plain_string version(version_t v)
{
    switch(v)
    {
    case version_t::v10:
        return "HTTP/1.0";
    case version_t::v11:
        return "HTTP/1.1";
    case version_t::v20:
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
    case header_field::content_disposition:
        return "Content-Disposition";
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
    case header_field::date:
        return "Date";
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
    case http::content_type::multipart_form:
        return "multipart/form-data";

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

inline plain_string method(method_t meth)
{
    switch(meth)
    {
    case http::method_t::delet:
        return "DELETE";
    case http::method_t::get:
        return "GET";
    case http::method_t::head:
        return "HEAD";
    case http::method_t::options:
        return "OPTIONS";
    case http::method_t::patch:
        return "PATCH";
    case http::method_t::post:
        return "POST";
    case http::method_t::put:
        return "PUT";
    case http::method_t::update:
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
inline version_t version(string const& v)
{
    if(v.substr(0, 5) != "HTTP/")
        return version_t::v10;

    auto ver_sub = v.substr(5, 3);

    if(ver_sub == "1.0")
        return version_t::v10;
    if(ver_sub == "1.1")
        return version_t::v11;
    if(ver_sub == "2.0")
        return version_t::v20;

    return version_t::v10;
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
    if(util::strings::iequals(f, "Content-Disposition"))
        return header_field::content_disposition;
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
    if(util::strings::iequals(f, "Date"))
        return header_field::date;
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

inline method_t method(string const& v)
{
    if(v == "DELETE")
        return method_t::delet;
    if(v == "GET")
        return method_t::get;
    if(v == "HEAD")
        return method_t::head;
    if(v == "OPTIONS")
        return method_t::options;
    if(v == "PATCH")
        return method_t::patch;
    if(v == "POST")
        return method_t::post;
    if(v == "PUT")
        return method_t::put;
    if(v == "UPDATE")
        return method_t::update;

    return method_t::get;
}

inline http::content_type content_type(string v)
{
    using c = http::content_type;

    auto v1 = v.find(";");
    v       = v.substr(0, v1);
    v       = stl_types::str::trim::both(v);

    if(v == "*/*")
        return c::any;
    if(v == "multipart/form-data")
        return c::multipart_form;

    if(v == "text/plain")
        return c::text;
    if(v == "application/json")
        return c::json;
    if(v == "application/xml")
        return c::xml;

    if(v == "application/octet-stream")
        return c::octet_stream;
    if(v.substr(0, 6) == "audio/")
        return c::audio;
    if(v.substr(0, 6) == "image/")
        return c::image;
    if(v.substr(0, 6) == "video/")
        return c::video;

    return c::any;
}
} // namespace from_string

namespace serialize {
/*!
 * \brief Outputs:
 * `[method] [resource] HTTP/[version]`
 * \param header
 * \return
 */
template<typename Traits = header_traits>
inline string request_line(header_t const& header)
{
    return to_string::method(header.method) + string(" ") + header.resource +
           " " + to_string::version(header.version) + Traits::header_separator;
}

/*!
 * \brief Outputs:
 * `HTTP/[version] [status code] [message]`
 * \param header
 * \return
 */
template<typename Traits = header_traits>
inline string response_line(header_t const& header)
{
    return to_string::version(header.version) + string(" ") +
           cast_pod(header.code) + " " + header.message +
           Traits::header_separator;
}

template<typename Traits = header_traits>
inline string field(string const& key, string const& value)
{
    return key + ": " + value + Traits::header_separator;
}

template<typename Traits = header_traits>
inline string fields(header_t const& header)
{
    string out;

    for(auto const& field : header.standard_fields)
        out += serialize::field<Traits>(
            to_string::field(field.first), field.second);

    for(auto const& field : header.fields)
        out += serialize::field<Traits>(field.first, field.second);

    return out;
}

template<typename Traits = header_traits>
inline string request(header_t const& header)
{
    string out;

    out += serialize::request_line<Traits>(header);
    out += serialize::fields<Traits>(header);
    out += Traits::header_separator;

    return out;
}

template<typename Traits = header_traits>
inline string response(header_t const& header)
{
    string out;

    out += serialize::response_line<Traits>(header);
    out += serialize::fields<Traits>(header);
    out += Traits::header_separator;

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

template<typename T>
inline szptr read_value(semantic::Span<T> const& buf, string& target)
{
    auto start = semantic::mem_chunk<const char>::ofContainer(buf).view;
    auto end = std::find(start.begin(), start.end(), ' ');

    if(end != start.end())
    {
        target.insert(0, start.data(), end - start.begin());
        return target.size() + 1;
    }

    return 0;
}

template<typename T>
inline szptr read_line(semantic::Span<T> const& buf, string& target)
{
    auto start = semantic::mem_chunk<const char>::ofContainer(buf);
    auto end   = start.find(semantic::mem_chunk<const char>::ofString("\r\n"));

    if(end != start.view.end() && (start.view.end() - end) > 0)
    {
        target.insert(0, start.view.data(), end - start.view.begin());
        return target.size() + 2;
    }

    return 0;
}

template<typename Rep>
inline Rep read_int(std::istream& h)
{
    auto value = read_value(h);
    return cast_string<Rep>(value);
}

template<typename Rep, typename T>
inline szptr read_int(semantic::Span<T> const& h, Rep& out)
{
    string tmp;
    szptr  off = read_value(h, tmp);
    out        = cast_string<Rep>(tmp);
    return off;
}

inline header_t& response_line(header_t& h, std::istream& s)
{
    /* Read in from:
     * HTTP/1.0 200 OK
     */

    h.version = header::from_string::version(read_value(s));
    h.code    = read_int<u16>(s);
    h.message = util::strings::trim(read_line(s));

    return h;
}

template<typename T>
inline szptr response_line(header_t& h, semantic::Span<T> const& buf)
{
    szptr offset = 0;

    {
        string tmp;
        offset += read_value(buf, tmp);
        h.version = header::from_string::version(tmp);
    }
    offset += read_int<u16>(buf.subspan(offset), h.code);
    offset += read_line(buf.subspan(offset), h.message);

    return offset;
}

inline header_t& request_line(header_t&, std::istream&)
{
    /* Read in from:
     * GET /resource HTTP/1.0
     */

    Throw(undefined_behavior("something's wrong here"));

    //    h.method   = from_string::method(read_value(s));
    //    h.resource = read_value(s);
    //    h.code     = read_int<u16>(s);
    //    h.message  = read_line(s);

    //    return h;
}

inline std::pair<string, string> field(string const& line)
{
    auto split = line.find(":");
    auto len   = line.size();

    auto key   = line.substr(0, split);
    auto value = line.substr(split + 1, len - split - 1);

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
    struct field_value
    {
        string m_value;

        string str() const
        {
            return m_value;
        }

        std::pair<string, string> pair() const
        {
            auto split = m_value.find('=');

            if(split == string::npos)
                return {};

            return {m_value.substr(0, split), m_value.substr(split + 1)};
        }
    };

    field_iterator() : m_pos(string::npos)
    {
    }

    field_iterator(string const& value, string::value_type sep) :
        m_value(" " + value), m_pos(0), m_separator(sep)
    {
    }

    bool operator==(field_iterator const& it) const
    {
        return it.m_pos == m_pos;
    }

    bool operator!=(field_iterator const& it) const
    {
        return it.m_pos != m_pos;
    }

    field_iterator& operator++()
    {
        if(m_pos == string::npos)
            Throw(std::out_of_range("no more values"));

        m_pos = m_value.find(m_separator, m_pos + 1);

        return *this;
    }

    field_value operator*()
    {
        auto next_value = m_value.find(m_separator, m_pos + 1);

        auto value = m_value.substr(m_pos + 1, next_value - 1);

        return {util::strings::trim(value)};
    }

  private:
    string             m_value;
    size_t             m_pos;
    string::value_type m_separator;
};
} // namespace parse

namespace transform {
inline header_t& create_continue(header_t& origin)
{
    versioning::ensure(origin, version_t::v11);

    origin.standard_fields[header_field::expect] = "100-Continue";

    return origin;
}

inline request_t& create_request(request_t& origin)
{
    auto& standard_headers = origin.header.standard_fields;

    auto host = origin.host;

    if(origin.port != 80 && origin.port != 443)
        host += ":" + cast_pod(origin.port);

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
        auto end       = m_query_start == string::npos ? string::npos
                                                       : m_query_start - m_pos + 1;
        auto next_comp = m_resource.find('/', m_pos + 1) - m_pos;

        return m_resource.substr(m_pos + 1, std::min(next_comp, end) - 1);
    }

    inline bool invalid() const
    {
        return m_pos == string::npos;
    }

  private:
    size_t m_pos;
    string m_resource;
    size_t m_query_start;
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

    size_t m_pos;
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

    if(rq.header.method == method_t::get && rq.payload.size() > 0)
        return result::semantic_error;

    return result::valid;
}
} // namespace validate

namespace stream {
using progress_fun = std::function<void(size_t, size_t)>;

inline header_t& read_header(header_t& out, std::istream& h)
{
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

inline header_t read_response(std::istream& h)
{
    header_t out = {};

    header::parse::response_line(out, h);

    read_header(out, h);

    return out;
}

inline payload_t read_payload_deterministic(
    std::istream& p, size_t len, progress_fun progress = nullptr)
{
    payload_t payload;
    payload.resize(len);

    auto payload_ptr = payload.data();
    i64  buf_size    = static_cast<i64>(payload.size());
    i64  offset      = 0;

    while(p.read(&payload_ptr[offset], buf_size - offset) && offset < buf_size)
    {
        if(progress)
            progress(payload.size(), len);
        offset += p.gcount();
    }

    return payload;
}

inline payload_t read_payload_indeterminate(
    std::istream& p, progress_fun progress)
{
    payload_t payload;
    size_t    offset = 0;

    constexpr size_t bufsize = 4096;

    // TODO: Find out how to detect EOF for HTTP(S) streams with std::istream
    while(!p.eof())
    {
        payload.resize(payload.size() + bufsize);
        auto read = p.readsome(&payload[offset], bufsize);

        if(progress)
            progress(payload.size(), payload.size());

        const auto read_diff = bufsize - C_FCAST<size_t>(read);

        if(read_diff > 0)
        {
            payload.resize(payload.size() - read_diff);
            break;
        }
    }

    return payload;
}

inline payload_t read_payload(
    std::istream& p, size_t len, progress_fun progress = nullptr)
{
    if(len == 0)
        return read_payload_indeterminate(p, progress);
    else
        return read_payload_deterministic(p, len, progress);
}

} // namespace stream

namespace buffer {

template<typename T>
inline szptr read_header(header_t& out, semantic::Span<T> const& buf)
{
    szptr  offset = 0;

    while(offset < buf.size())
    {
        string tmp;
        offset += header::parse::read_line(buf.subspan(offset), tmp);

        if(tmp.empty())
            break;

        auto classified = header::parse::field_classify(tmp);

        if(classified.first.first != header_field::none)
            out.standard_fields.insert(classified.first);
        else
            out.fields.insert(classified.second);
    }

    return offset;
}

template<typename T>
inline szptr read_response(header_t& out, semantic::Span<T> const& buf)
{
    szptr offset = 0;
    out          = {};

    offset += header::parse::response_line(out, buf);
    offset += read_header(out, buf.subspan(offset));
//    offset += 2;

    return offset;
}

} // namespace buffer

#if defined(COFFEE_HTTP_MULTIPART)
namespace multipart {
struct missing_terminator : std::out_of_range
{
    using out_of_range::out_of_range;
};

struct part_iterator
{
    using part = std::pair<header_t, payload_t>;
    static constexpr cstring multipart_terminator = "--";

    part_iterator(string bound, payload_t& payload) :
        m_boundary(bound), m_payload(payload), m_pos(0), m_ended(false)
    {
        ++(*this);
    }

    static inline part_iterator sentinel(payload_t& p)
    {
        part_iterator out("", p);
        out.m_ended = true;
        return out;
    }

    bool operator==(part_iterator const& other)
    {
        return m_ended == other.m_ended;
    }

    bool operator!=(part_iterator const& other)
    {
        return m_ended != other.m_ended;
    }

    size_t next_section()
    {
        if(m_payload.size() <= m_pos)
            return static_cast<size_t>(-1);

        void* ptr = ::memmem(
            &m_payload.at(m_pos),
            C_FCAST<size_t>(m_payload.size() - m_pos),
            m_boundary.data(),
            C_FCAST<size_t>(m_boundary.size()));

        if(ptr == nullptr)
        {
            auto remaining = m_payload.size() - m_pos;

            if(remaining < 2)
                Throw(
                    std::out_of_range("multipart body EOF without terminator"));

            /* Check for terminator */
            void* terminator_ptr = ::memmem(
                &m_payload.at(m_pos), remaining, multipart_terminator, 2);

            if(terminator_ptr == nullptr)
                Throw(missing_terminator("failed to locate terminator"));

            return static_cast<size_t>(-1);
        }

        return C_FCAST<size_t>(C_RCAST<char*>(ptr) - &m_payload.at(0));
    }

    part_iterator& operator++()
    {
        if(!m_ended)
        {
            m_pos = next_section() + m_boundary.size();

            if(next_section() == static_cast<size_t>(-1))
                m_ended = true;
        }

        return *this;
    }

    part operator*()
    {
        auto end = next_section();

        auto start = m_pos;

        auto len = end - start;

        header_t           header;
        std::istringstream header_strm(string(&m_payload.at(m_pos), len));

        auto first_char = header_strm.peek();

        if(first_char == http::line_separator[0] ||
           first_char == http::line_separator[1])
        {
            string discard;
            std::getline(header_strm, discard);
        }

        stream::read_header(header, header_strm);

        size_t header_len = C_FCAST<size_t, off_t>(header_strm.tellg());

        auto      payload_len = end - start - header_len;
        payload_t part_payload;
        part_payload.insert(
            part_payload.begin(),
            m_payload.begin() + C_FCAST<off_t>(m_pos + header_len),
            m_payload.begin() +
                C_FCAST<off_t>(m_pos + header_len + payload_len));

        return {header, part_payload};
    }

  private:
    string     m_boundary;
    payload_t& m_payload;
    size_t     m_pos;
    bool       m_ended;
};

struct parser
{
    string    m_boundary;
    payload_t m_payload;

    part_iterator m_sentinel;

    parser(string boundary, payload_t&& payload) :
        m_boundary(boundary), m_payload(std::move(payload)),
        m_sentinel(part_iterator::sentinel(m_payload))
    {
    }

    part_iterator begin()
    {
        return part_iterator(m_boundary, m_payload);
    }

    part_iterator& end()
    {
        return m_sentinel;
    }
};

struct builder
{
    builder(cstring terminator) : m_terminator(terminator)
    {
    }

    void add(
        string const&                         name,
        semantic::BytesConst const&           data,
        stl_types::Map<string, string> const& extra_headers)
    {
        /* TODO: Avoid copying the content here */
        m_data += "--";
        m_data += m_terminator;
        m_data += line_separator;
        m_data += header::serialize::field(
            "Content-Disposition", "form-data; name=\"" + (name + "\""));
        for(auto const& header : extra_headers)
            m_data += header::serialize::field(header.first, header.second);
        m_data += line_separator;
        m_data.insert(m_data.end(), data.begin(), data.end());
        m_data += line_separator;
    }
    void finalize()
    {
        m_data += "--";
        m_data += m_terminator;
        m_data += "--";
        m_data += line_separator;
        m_data += line_separator;
    }

    operator semantic::BytesConst() const
    {
        return semantic::BytesConst::ofContainer(m_data);
    }

    string content_type() const
    {
        return header::to_string::content_type(
                   http::content_type::multipart_form) +
               string("; boundary=") + m_terminator;
    }

    plain_string m_terminator;
    string       m_data;
};

} // namespace multipart
#endif

} // namespace http
