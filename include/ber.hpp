#pragma once

#include <vector>
#include <string>
#include <object.hpp>
#include <boost/spirit/include/qi.hpp>

enum BerTypes {
    INTEGER = 2,
    OCTET = 4,
    NUL = 5,
    OID = 6,
    SEQ = 16
};

class Encoder {
private:
    std::vector<unsigned char> encoded;
    Object* obj;
    std::string val;
    void encodeSize(int size);
    void encodeValue(int type, std::string value);
    void encodeEOC(void);
    void encodeOID(int oid);
    void encodeInt(long long value);
    void encodeTag(int cls, bool primitive, int n);
public:
    Encoder(Object* object, std::string value);
    void encode(void);
};