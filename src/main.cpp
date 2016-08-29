// Grabber: tool for download all links from given URL in parallel
// Author:  Ondřej Cvacho
// Libs:    libcurl-openssl-dev 7.47.0
//          zlib1g-dev
//          pthreads
//
// Restrictions:
//  - download dir must exist

#include "config.h"
#include "grabber.h"
#include "hash.h"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <algorithm>

void usage(void)
{
    std::cerr << "./grabber [-u URL] [-d DIR] [-t NUM]" << std::endl
                << "-u URL\tfile to parse (default: meetangee.com)" << std::endl
                << "-d DIR\tdownload directory (default: ./data)" << std::endl
                << "-t NUM\tnumber of threads to run" << std::endl
                << std::endl;
}

// Very simple function to parse the input
//
Config parse_input(int argc, char * argv[])
{
    std::string url = FILE_URL;
    std::string base = FILE_URL;
    std::string dir = DOWNLOAD_DIR;
    unsigned int threads = THREADS;

    int c;
    while ((c = getopt(argc, argv, "u:d:t:")) != -1)
    {
        switch (c)
        {
            case 'u':
                url = optarg;
                break;
            case 'd':
                dir = optarg;
                break;
            case 't':
            {
                std::istringstream stream(optarg);
                stream >> threads;
                break;
            }
            case '?':
                std::cerr << "Unsupported argument or argument syntax" << std::endl;
                usage();
                exit(1);
                break;
            default:
                std::cerr << "All undefined arguments will be set to default value" << std::endl;
                break;
        }
    }

    // Check if dir path ends with trailing '/'
    // or add it
    if (dir.back() != '/')
        dir += '/';

    // HACK to remove file name from URL
    size_t pos_s = url.find_first_of('/');
    size_t pos_e = url.find_last_of('/');

    if (pos_e != (pos_s + 1))
        base = url.substr(0, pos_e);

    return Config(url, dir, threads, base);
}

int main(int argc, char * argv[])
{
    Config config = parse_input(argc, argv);
    Adler32 adler32;

    std::vector<FileInfo> files;
    {
        Grabber grabber(config, adler32);

        try
        {
            grabber.run();
        }
        catch (std::exception & e)
        {
            std::cerr << "ERROR: Can't download given URL. Ending." << std::endl;
            return 1;
        }

        grabber.get_files_info(files);
    }

    if (files.size() == 0)
    {
        std::cout << "No downloaded files." << std::endl;
        return 0;
    }

    size_t min_size = 0;
    size_t max_size = 0;
    for (size_t i = 0; i < files.size(); i++)
    {
        if (files[i].size > files[max_size].size)
            max_size = i;

        if (files[i].size < files[min_size].size)
            min_size = i;

        std::cout << "file: " << files[i].name << std::endl
                    << "\thash: " << files[i].hash_str << std::endl;
    }

    std::cout << "SMALLEST = " << files[min_size].name << " ["
                << files[min_size].hash_str << "]" << std::endl
                << "LARGEST = " << files[max_size].name << " ["
                << files[max_size].hash_str << "]" << std::endl;

    std::cout << "files count: " << files.size() << std::endl;

    return 0;
}
