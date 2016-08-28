#include "io.h"

#include <fstream>

// stores content into file_name
// @param  file_name - any valid path to an output file
// @param  content   - data to be written into file_name
// @return           - if the write operation were successful
//
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
