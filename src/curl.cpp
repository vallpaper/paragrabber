#include "curl.h"

#include <curl/curl.h>

Curl::Curl()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

Curl::~Curl()
{
    curl_global_cleanup();
}

bool Curl::get(const std::string & url, std::string & data) const
{
    CURL * handle = curl_easy_init();
    CURLcode res;

    if (!handle)
        return false;

    // Set curl options
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(handle);

    curl_easy_cleanup(handle);

    if (res == CURLE_OK)
        return true;
    else
        return false;
}

// Callback function for storing recieved data into buffer
// @return            number of recieved bytes
//
size_t Curl::write_data(void * in_buffer, size_t size, size_t nmemb, std::string * out_buffer)
{
    size_t data_size = size * nmemb;

    if (out_buffer != NULL)
        out_buffer->append((char *) in_buffer, data_size);

    return data_size;
}
