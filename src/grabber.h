#ifndef __GRABBER_H__
#define __GRABBER_H__

#include "config.h"
#include "curl.h"
#include "hash.h"
#include "io.h"
#include "semaphore.h"

#include <deque>
#include <unordered_set>
#include <vector>
#include <string>
#include <mutex>
#include <thread>

// FileInfo struct
// - holds information about downloaded file
struct FileInfo
{
    FileInfo(const std::string t_name, const size_t t_size,
                const std::string t_hash_str, const std::string t_url)
        : name(t_name), size(t_size), hash_str(t_hash_str), url(t_url)
    {}

    // Get file name with hash of the file as a string
    //
    const std::string hashed_name() { return hash_str + "_" + name; };

    std::string     name;
    size_t          size;
    std::string     hash_str;
    std::string     url;
};

// Grabber class
// provides functionality to download all link from the given URL
// and calculate checksum/hash of the data.
//
// Names of files of downloaded links are made from their checksum/hash and URL file name
// example: [checksum]_[filenameurl]
class Grabber
{
public:
    Grabber(Config & config, Hasher & hasher);
    ~Grabber();

    // Download and parse input URL.
    // Creates and maintain threads
    void run();

    // Copy all file_info structures to files
    // We used pointers for internal "moving"
    // to reduce call of copy construct of FileInfo.
    void get_files_info(std::vector<FileInfo> & files);

private:
    // Helper method for starting threads
    //
    void start_threads();

    // Download method that threads runs
    //
    void consumer();

    // Convert URL into absolute URL if not already
    //
    std::string get_absolute_url(const std::string url) const;

    // Get file name from the URL without any special characters.
    // May return an empty string
    std::string get_file_name(const std::string url) const;

    // Download file from URL into CONTENT buffer
    // and stores into FILE_NAME
    FileInfo * download_and_save(std::string & content, const std::string & url,
                                            const std::string & file_name) const;

    std::deque<std::string>         m_files;
    std::unordered_set<std::string> m_files_visited;

    std::deque<FileInfo *>          m_files_info;

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
