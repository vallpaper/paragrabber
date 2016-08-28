#include "hash.h"

#include <zlib.h>

unsigned long Adler32::hash(const std::string & data) const
{
    const char * buffer = data.c_str();

    unsigned long adler = adler32(0UL, Z_NULL, 0);
    return adler32(adler, (Bytef *)buffer, data.size());
}
