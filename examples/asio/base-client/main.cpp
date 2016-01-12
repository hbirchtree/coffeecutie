#include <coffee/CCore>
#include <asio.hpp>
#include <iostream>

using namespace Coffee;

int32 coffee_main(int32, byte_t**)
{
    asio::io_service service;
    asio::ip::tcp::resolver resolver(service);

    asio::ip::tcp::resolver::query query("tmi.twitch.tv","https");

    auto it = resolver.resolve(query);

    asio::ip::tcp::socket sock(service);
    asio::connect(sock,it);

    asio::streambuf request;
    std::ostream req_stream(&request);
    req_stream << "GET " << "/get" << "HTTP/1.0\r\n";
    req_stream << "Host: " << "tmi.twitch.tv" << "\r\n";
    req_stream << "Accept: */*\r\n";
    req_stream << "Connection: close\r\n\r\n";

    asio::write(sock,request);

    asio::streambuf response;
    asio::read_until(sock,response,"\r\n");

    std::istream resp_stream(&response);
    std::string http_ver;
    resp_stream >> http_ver;
    uint32 stat_code;
    resp_stream >> stat_code;
    std::string msg;
    std::getline(resp_stream,msg);
    if(!resp_stream||http_ver.substr(0,5)!="HTTP/")
        return 1;
    if(stat_code!=200)
        return stat_code;
    else
        cDebug("Return code: {0}",stat_code);

    asio::read_until(sock,response,"\r\n\r\n");

    std::string head;
    while(std::getline(resp_stream,head)&&head!="\r")
        std::cout << head << std::endl;

    if(response.size() > 0)
        std::cout << &response;

    while(asio::read(sock,response,asio::transfer_at_least(1)))
        std::cout << &response;

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
