#include "httpParsingIncludes.hpp"
#include "HTTPRequestParser.hpp"

int main()
{
    std::string request = "GET /index.html HTTP/1.1\r\n"
                          "Host: www.example.com\r\n"
                          "User-Agent: Mozilla/5.0\r\n"
                          "Accept: text/html\r\n"
                          "\r\n";


    HTTPRequestParser parser(request);

    std::cout << "result: " << parser.getParsingCode() << std::endl;


    return 0;
}