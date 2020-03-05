#pragma once

#include <curl/curl.h>
#include <sstream>
#include <utility>

struct HttpResponse
{
    int httpStatus;
    std::string contents;
};

// Thank you Stack Overflow!
class HttpClient
{
private:
    CURL* curl;
    std::stringstream ss;
    int httpCode;

public:
    HttpClient();
    ~HttpClient();
    HttpResponse Get(const std::string& url);

private:
    static size_t _WriteData(void* buffer, size_t size, size_t nmemb, void* userp);
    size_t _Write(void* buffer, size_t size, size_t nmemb);
};
