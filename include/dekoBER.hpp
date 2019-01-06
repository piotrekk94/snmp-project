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
    std::vector<BerObject*> children;
public:
    void Display(int depth);
    BerObject(uint8_t objClass, uint8_t objType, uint64_t objTag, Type *objMIBType, uint64_t objLen, std::vector<uint8_t> &objData) {
        this->objClass = objClass; this->objType = objType; this->objTag = objTag; this->objMIBType = objMIBType; this->objLen = objLen; this->objData.assign(objData.begin(), objData.end());
    };
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
    void Display(std::vector<uint8_t> &data);
    void Display(BerObject *obj);
public:
    DekoBER(std::vector<uint8_t> &encodedData);
};