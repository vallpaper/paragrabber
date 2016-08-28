#include "grabber.h"

#include <fstream>
#include <regex>
#include <cctype>
#include <vector>

#include <sstream>
#include <thread>

Grabber::Grabber(Config & config, Hasher & hasher)
    : m_config(config), m_hasher(hasher)
{
    m_threads = new std::thread[m_config.threads];
}

Grabber::~Grabber()
{
    for (auto & file_info : m_files_info )
        delete file_info;

    delete [] m_threads;
}

FileInfo * Grabber::download_and_save(std::string & content, const std::string & url,
                                        const std::string & file_name) const
{
    m_curl.get(url, content);

    unsigned int hash = m_hasher.hash(content);

    // Convert ulong int into hex representation string
    std::stringstream hash_str;
    hash_str << std::hex << hash;

    FileInfo * info = new FileInfo(file_name, content.size(), hash_str.str(), url);

    m_io.write_to_file(m_config.download_dir + info->hashed_name(), content);

    return info;
}

void Grabber::run()
{
    start_threads();

    // (1) Download given file and save
    std::string input;

    std::string file_name = get_file_name(m_config.file_url);
    file_name = (file_name == "") ? "index.html" : file_name;

    FileInfo * info = download_and_save(input, m_config.file_url, file_name);
    m_files_info.push_back(info);
    m_files_visited.insert(m_config.file_url);

    // (2) Parse file and find all links
    std::regex url_regex("(href|src)=\"[^#][^\"]+\"");
    std::smatch match;

    std::sregex_iterator next(input.begin(), input.end(), url_regex);
    std::sregex_iterator end;

    for (; next != end; next++)
    {
        match = *next;
        std::string absolute_url = get_absolute_url(match.str());

        if (m_files_visited.find(absolute_url) != m_files_visited.end())
            continue;

        m_files_visited.insert(absolute_url);

        // Critical section
        // Add URL to worker threads
        {
            std::unique_lock<std::mutex> lock(m_mutex_files);
            m_files.push_back(absolute_url);
        }
        // End of critical section

        // Increase semaphore
        m_sem.notify();
    }

    // Update counter by number of threads
    // threads will pass the semaphore and end
    for (unsigned int i = 0; i < m_config.threads; i++)
        m_sem.notify();

    // Wait until all threads ends
    for (unsigned int i = 0; i < m_config.threads; i++)
        m_threads[i].join();
}

void Grabber::consumer()
{
    std::string url;

    // Store information about downloaded files by this thread
    // and at the end copy into object member m_files_info
    // to reduce number of mutex locks
    std::deque<FileInfo *> files_info;

    while (true)
    {
        // wait for work - only main thread increases
        m_sem.wait();

        // Critical section
        {
            std::unique_lock<std::mutex> lock(m_mutex_files);

            if (m_files.size() == 0)
                break;

            url = m_files.front();
            m_files.pop_front();
        }
        // End of critical section

        std::string content;

        FileInfo * info = download_and_save(content, url, get_file_name(url));
        files_info.push_back(info);
    }

    // move all elements from files_info into m_files_info
    {
        std::unique_lock<std::mutex> lock(m_mutex_files_info);
        m_files_info.insert(m_files_info.end(), files_info.begin(), files_info.end());
    }
}

void Grabber::start_threads()
{
    for (unsigned int i = 0; i < m_config.threads; i++)
        m_threads[i] = std::thread(&Grabber::consumer, this);
}

// Convert given URL into absolute URL
// relative urls "/assets/..." => "example.com/assets/..."
// absolute URL untouched
std::string Grabber::get_absolute_url(const std::string url) const
{
    size_t pos_s = url.find_first_of("\"");
    size_t pos_e = url.find_last_of("\"");

    if (pos_s == std::string::npos || pos_e == std::string::npos || pos_e <= pos_s)
        return "";

    // increment position after first " character
    pos_s++;

    std::string url_part = url.substr(pos_s, pos_e - pos_s);

    if (url_part.length() == 0)
        return "";

    // two possible URLs
    // "//.." or "http..." -> absolute URLs
    // "/..", './', '../' -> relative URLs - needs to appends to input URL
    if (url[pos_s] == '.' || (url[pos_s] == '/' && url[pos_s + 1] != '/'))
    {
        // std::cout << m_config.base + url_part << std::endl;
        return m_config.base + url_part;
    }
    else if (url[pos_s] == '/' && url[pos_s + 1] == '/')
        return "http:" + url_part;

    return url_part;
}

// find last '/' and take everything until some special nasty characters
// that could be possible problem for file systems
//
std::string Grabber::get_file_name(const std::string url) const
{
    size_t pos_s = url.find_last_of('/');
    if (pos_s == std::string::npos)
        return "";

    pos_s++;
    size_t pos_e = pos_s;

    while (pos_e < url.size() && (isalnum(url[pos_e]) || url[pos_e] == '.'
            || url[pos_e] == '_' || url[pos_e] == '-'))
        pos_e++;

    if (pos_e <= pos_s)
        return "";

    return url.substr(pos_s, pos_e - pos_s);
}

void Grabber::get_files_info(std::vector<FileInfo> & files)
{
    for (auto const & file_info : m_files_info)
        files.push_back(*file_info);
}
