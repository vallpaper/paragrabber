#ifndef __DOWNLOADER_H__
#define __DOWNLOADER_H__

#include <string>

// Curl class
// - using libcurl for downloading files from given URL
//
class Curl
{
public:
    Curl();
    ~Curl();

    // Download file from the URL and stores into DATA
    // @param  url  [in]
    // @param  data [in/out]
    bool get(const std::string & url, std::string & data) const;
private:
    static size_t write_data(void * in_buffer, size_t size, size_t nmemb, std::string * out_buffer);
};

#endif /* end of include guard: __DOWNLOADER_H__ */
