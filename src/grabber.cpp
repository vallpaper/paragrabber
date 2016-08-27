#include "grabber.h"

#include <fstream>
#include <regex>
#include <cctype>

#include <iostream> // test

Grabber::Grabber(Config & config, Hasher & hasher)
    : m_config(config), m_hasher(hasher)
{
}

void Grabber::run()
{
    // download and parse given input file
    // and create threads

    // (1) Download given file
    // TODO
    RecvData web;
    m_curl.get(m_config.file_url, web);

    std::cout << "After CURL " << web.raw.size() << "\n";

    // (2) Parse file and find all links
    // TODO

    // load file into memory TODO - zbytecne
    std::string input = web.raw;
    // get_file_content("data/index.html", input);

    std::regex url_regex("(href|src)=\"[^#][^\"]+\"");
    std::smatch match;

    std::sregex_iterator next(input.begin(), input.end(), url_regex);
    std::sregex_iterator end;

    for (; next != end; next++)
    {
        match = *next;
        std::string absolute_url = get_absolute_url(match.str());

        // std::cout << "URL: " << match.str() << " => " << absolute_url << "\n";

        if (m_files_visited.find(absolute_url) != m_files_visited.end())
            continue;

        m_files_visited.insert(absolute_url);

        // TODO: mutex
        m_files.push_back(absolute_url);

        // TODO: semaphore rise
    }

    std::cout << "Found URLs: " << m_files.size() << "\n";

    // wait for threads
    consumer();
}

// void Grabber::producer()
// {
// }

void Grabber::consumer()
{
    RecvData packet;
    std::string url;

    while (true)
    {
        if (m_files.size() == 0)
            break;

        // TODO: mutex
        url = m_files.front();
        m_files.pop_front();
        // Mutex end

        // get the file
        std::cout << "downloading: " << url << "\n";
        m_curl.get(url, packet);
        std::cout << "\tsize: " << packet.raw.size() << "\n";
        std::cout << "\thash: " << m_hasher.hash(packet.raw) << "\n";
        std::cout << "\tfile: " << get_file_name(url) << "\n";
    }
}

void Grabber::get_file_content(const std::string & file_name, std::string & content)
{
    std::ifstream in_file(file_name);

    if (!in_file.is_open())
        return; // throw exception

    in_file.seekg(0, std::ios::end);
    content.resize(in_file.tellg());
    in_file.seekg(0, std::ios::beg);
    in_file.read(&content[0], content.length());

    in_file.close();
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
            || url[pos_e] == '_'))
        pos_e++;

    if (pos_e <= pos_s)
        return "";

    return url.substr(pos_s, pos_e - pos_s);
}
