#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>

#define FILE_URL        "meetangee.com"
#define DOWNLOAD_DIR    "download"
#define THREADS         4

struct Config
{
    Config()
        : file_url(FILE_URL), download_dir(DOWNLOAD_DIR),
            threads(THREADS)
    {};

    Config(std::string t_file_url, std::string t_download_dir, int t_threads)
        : file_url(t_file_url), download_dir(t_download_dir),
            threads(t_threads)
    {};

    // stores given URL path example.com/some/path
    std::string file_url = "meetangee.com";

    // stores domain name - example.com
    std::string domain = "meetangee.com";

    // where to download all files
    std::string download_dir = "data";

    // number of threads to run
    int         threads = 4;
};

#endif /* end of include guard: __CONFIG_HPP__ */
