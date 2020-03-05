#include <iostream>
#include "Http.h"

// Thank you Stack Overflow!

HttpClient::HttpClient() :
    curl(curl_easy_init()), httpCode(0)
{}

HttpClient::~HttpClient()
{
    if (curl)
        curl_easy_cleanup(curl);
}

HttpResponse HttpClient::Get(const std::string& url)
{
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return { 0, curl_easy_strerror(res) };
    
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    std::string s = ss.str();
    ss.str("");
    return { httpCode, s };
}

size_t HttpClient::_WriteData(void* buffer, size_t size, size_t nmemb, void* userp)
{
    return static_cast<HttpClient*>(userp)->_Write(buffer, size, nmemb);
}

size_t HttpClient::_Write(void* buffer, size_t size, size_t nmemb)
{
    ss.write((const char*)buffer, size * nmemb);
    return size * nmemb;
}
