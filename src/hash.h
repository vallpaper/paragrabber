#ifndef __HASH_H__
#define __HASH_H__

#include <cstdint>
#include <string>

// Hasher class
// abstract class to support simple swap of concrete hash algorithm
class Hasher
{
public:
    Hasher() {};
    virtual unsigned long hash(const std::string & data) const = 0;
};

// Adler32 class
// calculate Adler32 checksum of the input
class Adler32 : public Hasher
{
public:
    Adler32() {};

    // Calculate Adler32 checksum of given data
    // @return      Adler32 checksum
    //
    virtual unsigned long hash(const std::string & data) const;
};

#endif /* end of include guard: __HASH_H__ */
