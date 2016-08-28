#include "io.h"

#include <fstream>

bool IOBuffer::write_to_file(const std::string & file_name, const std::string & content) const
{
    std::ofstream out_file(file_name);

    out_file << content;

    bool ret = true;
    if (!out_file.good())
        ret = false;

    out_file.close();
    return ret;
}
