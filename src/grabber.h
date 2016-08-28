#ifndef __GRABBER_H__
#define __GRABBER_H__

#include "config.h"
#include "curl.h"
#include "hash.h"
#include "io.h"
#include "semaphore.h"

#include <deque>
#include <string>
#include <unordered_set>
#include <set>
#include <mutex>
#include <thread>

struct FileInfo
{
    FileInfo(const std::string t_name, const size_t t_size,
                const std::string t_hash_str)
        : name(t_name), size(t_size), hash_str(t_hash_str)
    {}

    const std::string full_name() { return hash_str + "_" + name; };

    std::string     name;
    size_t          size;
    std::string     hash_str;
};

class Grabber
{
public:
    Grabber(Config & config, Hasher & hasher);
    ~Grabber();

    void run();

private:
    void start_threads();
    void consumer(unsigned int i);

    std::string get_absolute_url(const std::string url) const;
    std::string get_file_name(const std::string url) const;

    std::deque<std::string>         m_files;
    std::unordered_set<std::string> m_files_visited;

    std::unordered_set<FileInfo *>  m_files_info;

    const Config &                  m_config;
    const Hasher &                  m_hasher;
    const Curl                      m_curl;
    const IOBuffer                  m_io;

    // threads container
    std::thread *                   m_threads;

    // thread synchronization
    Semaphore                       m_sem;
    std::mutex                      m_mutex_files;
    std::mutex                      m_mutex_files_info;
};

#endif /* end of include guard: __GRABBER_H__ */
