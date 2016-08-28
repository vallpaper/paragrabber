#ifndef __IO_H__
#define __IO_H__

#include <string>

class IOBuffer
{
public:
    IOBuffer() {};
    // TODO: better return type
    bool write_to_file(const std::string & file_name, const std::string & content) const;
private:
};

#endif /* end of include guard: __IO_H__ */
