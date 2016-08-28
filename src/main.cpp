// libcurl-openssl-dev 7.47.0
// libcurl
// libcrypto++-dev

#include "config.h"
#include "grabber.h"
#include "hash.h"

#include <iostream>
#include <unistd.h>
#include <sstream>

void usage(void)
{
    std::cout << "./grabber [-u URL] [-d DIR] [-t NUM]" << "\n"
                << "-u URL\tfile to parse (default: meetangee.com)" << "\n"
                << "-d DIR\tdownload directory (default: ./data)" << "\n"
                << "-t NUM\tnumber of threads to run" << "\n"
                << std::endl;
}

// Very simple function to parse the input
//
Config parse_input(int argc, char * argv[])
{
    std::string url = FILE_URL;
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
                std::istringstream i(optarg);
                i >> threads;
                break;
            }
            case '?':
                std::cout << "Unsupported argument or argument syntax\n";
                usage();
                exit(1);
                break;
            default:
                std::cout << "All undefined arguments will be set default value\n";
                break;
        }
    }

    return Config(url, dir, threads);
}

int main(int argc, char * argv[])
{
    Config config = parse_input(argc, argv);
    Adler32 adler32;
    Grabber grabber(config, adler32);

    // Where all the magic happens
    grabber.run();

    return 0;
}
