#ifndef __GRABBER_H__
#define __GRABBER_H__

#include "config.h"
#include "curl.h"
#include "hash.h"

#include <deque>
#include <string>
#include <unordered_set>

class Grabber
{
public:
    Grabber(Config & config, Hasher & hasher);

    void run();

    // Opens given file by file_name and returns its content in content variable
    // @params:
    //  - file_name in
    //  - content   in/out
    // @return:
    //  - size_t - file size or -1 if fails
    // TODO: move somewhere else
    static void get_file_content(const std::string & file_name, std::string & content);

private:
    // void producer();
    void consumer();

    std::string get_absolute_url(const std::string url) const;
    std::string get_file_name(const std::string url) const;

    std::deque<std::string>    m_files;
    std::unordered_set<std::string> m_files_visited;

    Config &                    m_config;
    Hasher &                    m_hasher;
    Curl                        m_curl;
};

#endif /* end of include guard: __GRABBER_H__ */
