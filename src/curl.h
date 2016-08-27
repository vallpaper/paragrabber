#ifndef __DOWNLOADER_H__
#define __DOWNLOADER_H__

// To make download file using libcurl more simple
// TODO: comments

#include <string>

struct RecvData
{
    std::string raw;
    std::string name;
};

class Curl
{
public:
    Curl();
    ~Curl();

    int get(std::string const & url, RecvData & data) const;
private:
    static size_t write_data(void * in_buffer, size_t size, size_t nmemb, std::string * out_buffer);
};

#endif /* end of include guard: __DOWNLOADER_H__ */
