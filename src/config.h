#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>

#define FILE_URL        "meetangee.com"
#define DOWNLOAD_DIR    "download/"
#define THREADS         4

struct Config
{
    Config()
        : file_url(FILE_URL), base(FILE_URL),
            download_dir(DOWNLOAD_DIR), threads(THREADS)
    {};

    Config(std::string t_file_url, std::string t_download_dir,
            int t_threads, std::string t_base)
        : file_url(t_file_url), base(t_base),
            download_dir(t_download_dir), threads(t_threads)
    {};

    // stores given URL path example.com/some/path
    std::string     file_url;

    // stores base name - example.com
    std::string     base;

    // where to download all files
    std::string     download_dir;

    // number of threads to run
    unsigned int    threads;
};

#endif /* end of include guard: __CONFIG_HPP__ */
