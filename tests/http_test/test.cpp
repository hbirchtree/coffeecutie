#include <coffee/asio/http.h>
#include <coffee/asio/net_resource.h>
#include <coffee/core/CFiles>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool url_parsing()
{
    const Url queryUrl = "http://def.ghi/something/other/than"
                         "?query=abc"
                         "&filter=123"
                         "&another_thing=abc123"_web;

    for(C_UNUSED(auto _) : Range<>(10))
    {
        ProfContext __("Parse");

        Profiler::PushContext("URL parsing");
        auto components = UrlParse::From(queryUrl);
        Profiler::PopContext();

        auto path_it = http::resource::path_iterator(components.resource());

        assertEquals(*path_it, "something");

        ++path_it;
        assertEquals(*path_it, "other");

        ++path_it;
        assertEquals(*path_it, "than");
    }

    return true;
}

bool url_query_parsing()
{
    CString baseUrl = "http://def.ghi/abc";

    http::resource::add_query(baseUrl, "search", "values123");
    assertEquals(baseUrl, "http://def.ghi/abc?search=values123");

    http::resource::add_query(baseUrl, "fil", "abc");
    assertEquals(baseUrl, "http://def.ghi/abc?search=values123&fil=abc");

    auto baseIt = http::resource::query_iterator(baseUrl);

    assert::Equals(*baseIt, {"search", "values123"});

    ++baseIt;
    assert::Equals(*baseIt, {"fil", "abc"});

    return true;
}

bool header_field_list()
{
    auto it = http::header::parse::field_iterator(
        "application/xml;  charset=utf-8", ';');

    assertEquals((*it).str(), "application/xml");

    ++it;
    assertEquals((*it).str(), "charset=utf-8");

    assertTrue(it != http::header::parse::field_iterator());

    ++it;

    assertTrue(it == http::header::parse::field_iterator());

    return true;
}

bool request_process()
{
    return true;
}

static constexpr cstring test_header_response =
    R"(HTTP/1.1 200 Some long message
Accept: application/json
X-Some-Extension: abc; charset=utf8
)";

bool response_process()
{
    const auto encoded_header =
        str::replace::str<char>(test_header_response, "\n", "\r\n");

    std::istringstream header_socket(encoded_header);

    auto header = http::stream::read_response(header_socket);

    assertEquals(header.message, "Some long message");
    assertEquals(header.code, 200);
    assertEquals(header.version, http::version_t::v11);

    /* Header fields */
    {
        using field = http::header_field;
        using namespace http::header;

        assertEquals(
            header.standard_fields[field::accept],
            to_string::content_type(http::content_type::json));

        assertEquals(header.fields["X-Some-Extension"], "abc; charset=utf8");
    }

    return true;
}

static constexpr cstring test_header_with_body =
    R"(HTTP/1.1 200 Some Long Message Again
Accept: */*
Content-Length: 85

here's some bullshit to read

even on multiple lines, my dude


and here it ends)";

bool response_read_payload()
{
    using field = http::header_field;

    const auto encoded_response =
        str::replace::str<char>(test_header_with_body, "\n", "\r\n");

    {
        std::istringstream socket(encoded_response);

        auto header = http::stream::read_response(socket);

        auto payload = http::stream::read_payload(socket, 0);

        assertEquals(payload.size(), 85);
    }

    {
        std::stringstream socket(encoded_response);

        auto header = http::stream::read_response(socket);

        auto payload_size =
            cast_string<szptr>(header.standard_fields[field::content_length]);

        assertEquals(payload_size, 85);

        auto payload = http::stream::read_payload(socket, 85);

        assertEquals(payload.size(), 85);
    }

    return true;
}

#if defined(COFFEE_HTTP_MULTIPART)
static constexpr cstring test_multipart_body =
    R"(this text is garbage and won't be included
some more

new line
--------------abc
Content-Type: text/plain; charset=utf8

abc
--------------abc
Content-Length: 3
Content-Disposition: form-data; filename="something.txt"

abc
--------------abc

--------------abc--
more garbage
don't read this
)";

bool multipart_process()
{
    const auto encoded_response =
        str::replace::str<char>(test_multipart_body, "\n", "\r\n");

    Vector<char> payload(encoded_response.begin(), encoded_response.end());

    auto parser =
        http::multipart::parser("--------------abc", std::move(payload));

    size_t num_parts = 0;

    for(auto part : parser)
    {
        cDebug("Part:");
        cDebug("-Headers:");
        for(auto field : part.first.standard_fields)
            cDebug(
                "--{0}: {1}",
                http::header::to_string::field(field.first),
                field.second);
        cDebug("-Payload size: {0}", part.second.size());
        num_parts++;
    }

    assertEquals(num_parts, 3);

    return true;
}
#else
bool multipart_process()
{
    return true;
}
#endif

COFFEE_TESTS_BEGIN(7)

    {url_parsing, "Parse HTTP URL"},
    {url_query_parsing, "Parsing HTTP URL queries"},
    {header_field_list, "Header field value parsing"},
    {request_process, "Request parsing"},
    {response_process, "Response parsing"},
    {response_read_payload, "Read response with payload"},
    {multipart_process, "Multiparty body processing"}

COFFEE_TESTS_END()
