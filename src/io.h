#ifndef __IO_H__
#define __IO_H__

#include <string>

// IOBuffer class
// - provides functionality for writing data into files
//
class IOBuffer
{
public:
    IOBuffer() {};

    // stores data in CONTENT into file FILE_NAME
    //
    bool write_to_file(const std::string & file_name, const std::string & content) const;
private:
};

#endif /* end of include guard: __IO_H__ */
