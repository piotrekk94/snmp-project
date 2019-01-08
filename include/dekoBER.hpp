#pragma once
#include <vector>
#include <cstdint>
#include <object.hpp>

class BerObject {
private:
    uint8_t objClass;
    uint8_t objType;
    uint64_t objTag;
    Type *objMIBType;
    uint64_t objLen;
    std::vector<uint8_t> objData;
    uint64_t DecodeVLQ(std::size_t &currentOctet, std::vector<uint8_t> &encodedData);
public:
    std::vector<BerObject*> children;
    int64_t AsInt(void);
    void AsInt(int depth);
    void AsOid(int depth);
    void AsStr(int depth);
    void AsOid(std::vector<uint64_t> &oid);
    void AsStr(std::string &str);
    void Display(int depth);
    uint8_t GetClass(void){return objClass;};
    uint8_t GetType(void){return objType;};
    uint64_t GetTag(void){return objTag;};
    std::vector<uint8_t> &GetData(void){return objData;};
    BerObject(uint8_t objClass, uint8_t objType, uint64_t objTag, Type *objMIBType, uint64_t objLen, std::vector<uint8_t> &objData) {
        this->objClass = objClass; this->objType = objType; this->objTag = objTag; this->objMIBType = objMIBType; this->objLen = objLen; this->objData.assign(objData.begin(), objData.end());
    };
    ~BerObject(void){for(auto &val : children) delete val;};
    void AddChild(BerObject *child) {this->children.push_back(child);}
};

class BerTag {
    static constexpr const char* const types[] = {"INTEGER", "OCTET STRING", "NULL", "OBJECT IDENTIFIER", "SEQUENCE", "OTHER"};
public:
    BerTag() {};
    const char * operator[](std::size_t idx) {
        switch(idx){
            case 2:
                return types[0];
            case 4:
                return types[1];
            case 5:
                return types[2];
            case 6:
                return types[3];
            case 16:
                return types[4];
            default:
                return types[5];
        }
    };
};

class DekoBER {
private:
    BerObject *root;
    void Decode(std::vector<uint8_t> &encodedData, BerObject *parent);
    uint64_t DecodeVLQ(std::size_t &currentOctet, std::vector<uint8_t> &encodedData);
    void Display(std::vector<uint8_t> &data);
    void Display(BerObject *obj);
public:
    DekoBER(std::vector<uint8_t> &encodedData);
    BerObject *GetBerTree(void) {return root;};
};