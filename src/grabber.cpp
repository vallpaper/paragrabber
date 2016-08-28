#include "grabber.h"

#include <fstream>
#include <regex>
#include <cctype>

#include <iostream> // test
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

void Grabber::run()
{
    start_threads();
    // download and parse given input file
    // and create threads

    // (1) Download given file
    // TODO
    std::string input;
    m_curl.get(m_config.file_url, input);

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

        // TODO: mutex
        {
            std::unique_lock<std::mutex> lock(m_mutex_files);
            m_files.push_back(absolute_url);
        }

        // TODO: semaphore rise
        m_sem.notify();
    }

    // Update counter by number of threads
    // - after all URLs downloaded they will pass semaphore
    //      to the break statement
    for (unsigned int i = 0; i < m_config.threads; i++)
        m_sem.notify();

    std::cout << "Found URLs: " << m_files_visited.size() << "\n";

    for (unsigned int i = 0; i < m_config.threads; i++)
        m_threads[i].join();

    // Final list print and smallest && largest files marked
    for (auto const & file_info : m_files_info)
        std::cout << "file: " << file_info->full_name() << " hash: " << file_info->hash_str << "\n";
}

void Grabber::consumer(unsigned int id)
{
    std::cout << "Started: " << id << "\n";

    std::string url;

    // For gathering into local set instead always lock mutex
    // At the end copy all to the object member m_files_info
    std::deque<FileInfo *> files_info;

    while (true)
    {
        // wait for work
        m_sem.wait();

        std::cout << "Semaphore passed: " << id << "\n";

        // get work
        {
            std::unique_lock<std::mutex> lock(m_mutex_files);

            if (m_files.size() == 0)
                break;

            url = m_files.front();
            m_files.pop_front();
        }

        std::string content;

        // get the file
        std::cout << "downloading: " << url << "\n"
                    << "\tthread: " << id << std::endl;

        m_curl.get(url, content);

        unsigned long hash = m_hasher.hash(content);

        std::stringstream hash_str;
        hash_str << std::hex << hash;

        FileInfo * info = new FileInfo(get_file_name(url), content.size(),
                                        hash_str.str());
        files_info.push_back(info);

        // TODO: ugly - path creation
        m_io.write_to_file(m_config.download_dir + info->full_name(), content);
    }

    // move all elements from files_info into m_files_info
    {
        std::unique_lock<std::mutex> lock(m_mutex_files_info);
        m_files_info.insert(files_info.begin(), files_info.end());
    }
}

void Grabber::start_threads()
{
    for (unsigned int i = 0; i < m_config.threads; i++)
        m_threads[i] = std::thread(&Grabber::consumer, this, i);
}

// void Grabber::get_file_content(const std::string & file_name, std::string & content)
// {
//     std::ifstream in_file(file_name);
//
//     if (!in_file.is_open())
//         return; // throw exception
//
//     in_file.seekg(0, std::ios::end);
//     content.resize(in_file.tellg());
//     in_file.seekg(0, std::ios::beg);
//     in_file.read(&content[0], content.length());
//
//     in_file.close();
// }

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
    // "/..", './', '../' -> relative URL - needs to appends to input URL
    if (url[pos_s] == '.' || (url[pos_s] == '/' && url[pos_s + 1] != '/'))
    {
        return m_config.domain + url_part;
    }
    else if (url[pos_s] == '/' && url[pos_s + 1] == '/')
        return "http:" + url_part;

    return url_part;
}

std::string Grabber::get_file_name(const std::string url) const
{
    // find last '/' and take everything until some special nasty characters
    // that could be possible problem for file systems (at least good practice)

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
