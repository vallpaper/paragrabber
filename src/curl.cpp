#include "curl.h"
#include "exception.h"

#include <curl/curl.h>

// #include <iostream>

Curl::Curl()
{
    // libcurl global initialization
    // need to be done before anything
    curl_global_init(CURL_GLOBAL_ALL);
}

Curl::~Curl()
{
    curl_global_cleanup();
}

int Curl::get(std::string const & url, RecvData & data) const
{
    CURL * handle = curl_easy_init();
    CURLcode res;

    if (!handle) // TODO: exception types
        throw new Exception();

    // URL set
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    // Set callback function for recieved data
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);

    // Set data where to write
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data.raw);

    // Perform action
    res = curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    if (res == CURLE_OK)
    {
        return 0;
    }
    else
        throw new Exception();
}

size_t Curl::write_data(void * in_buffer, size_t size, size_t nmemb, std::string * out_buffer)
{
    size_t data_size = size * nmemb;

    if (out_buffer != NULL)
        out_buffer->append((char *) in_buffer, data_size);

    return data_size;
}
