// libcurl-openssl-dev 7.47.0
// libcurl
// libcrypto++-dev

#include "config.h"
#include "grabber.h"
#include "hash.h"

#include <iostream>
#include <fstream>

void usage(void)
{
    std::cout << "./grabber [-u URL] [-d DIR]" << std::endl
                << "-u URL\tfile to parse (default: meetangee.com)" << std::endl
                << "-d DIR\tdownload directory (default: ./data)" << std::endl
                // << "\t-r\trecursive"
                << std::endl;
}

int main(int argc, const char * argv[])
{
    // TODO: input parse to config modification
    Config config;
    Adler32 adler32;

    Grabber grabber(config, adler32);

    grabber.run();

    return 0;
}
