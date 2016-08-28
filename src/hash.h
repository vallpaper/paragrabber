#ifndef __HASH_H__
#define __HASH_H__

// Create abstract hasher to simple swap hash function

#include <cstdint>
#include <string>

class Hasher
{
public:
    Hasher() {};
    virtual unsigned long hash(const std::string & data) const = 0;
};

class Adler32 : public Hasher
{
public:
    Adler32() {};
    virtual unsigned long hash(const std::string & data) const;
};

#endif /* end of include guard: __HASH_H__ */
