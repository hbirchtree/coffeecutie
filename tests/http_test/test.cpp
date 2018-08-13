#include <coffee/asio/http.h>
#include <coffee/core/CFiles>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool url_parsing()
{
    const Url queryUrl = "http://def.ghi/something/other/than"
                         "?query=abc"
                         "&filter=123"
                         "&another_thing=abc123"_web;

    auto components = UrlParse::From(queryUrl);

    auto it = http::resource::query_iterator(components.resource());

    assert::Equals(*it, {"query", "abc"});

    ++it;
    assert::Equals(*it, {"filter", "123"});

    ++it;
    assert::Equals(*it, {"another_thing", "abc123"});

    auto path_it = http::resource::path_iterator(components.resource());

    assertEquals(*path_it, "something");

    ++path_it;
    assertEquals(*path_it, "other");

    ++path_it;
    assertEquals(*path_it, "than");

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

    assertEquals(*it, "application/xml");

    ++it;
    assertEquals(*it, "charset=utf-8");

    return true;
}

bool request_process()
{
    return true;
}

bool response_process()
{
    return true;
}

COFFEE_TEST_SUITE(4) = {{url_parsing, "Parse HTTP resources"},
                        {header_field_list, "Header field value parsing"},
                        {request_process, "Request parsing + serialization"},
                        {response_process, "Response parsing + serialization"}};

COFFEE_EXEC_TESTS()
