#pragma once
#include <vector>
#include <string>
#include <cstdint>

enum MibObjectDataTypes {
    NONE,
    INTEGER,
    BINARY,
    OID,
    NUL,
};

class MibObject {
public:
    uint8_t objClass;
    uint8_t objType;
    uint64_t objTag;

    MibObjectDataTypes dataType;
    std::vector<uint8_t> binData;
    int64_t intData;
    std::vector<uint64_t> oidData;

    std::vector<MibObject*> children;

    MibObject(uint8_t objClass, uint8_t objType, uint64_t objTag) {this->objClass = objClass; this->objType = objType; this->objTag = objTag; dataType = NONE;};
    void setIntData(int64_t data) {if(dataType != NONE) return; intData = data; dataType = INTEGER;}
    void setBinData(std::vector<uint8_t> data) {if(dataType != NONE) return; binData = data; dataType = BINARY;}
    void setOidData(std::vector<uint64_t> data) {if(dataType != NONE) return; oidData = data; dataType = OID;}
    void AddChild(MibObject *child) {this->children.push_back(child);}
    void Display(int depth);
};

class KoBER {
private:
    std::vector<uint8_t> berData;
    void Encode(MibObject *obj, std::vector<uint8_t> &encodedData);
    void EncodePayload(MibObject *obj, std::vector<uint8_t> &encodedData);
    void EncodeOid(std::vector<uint64_t> &oidData, std::vector<uint8_t> &encodedData);
    void EncodeInt(int64_t intData, std::vector<uint8_t> &encodedData);
    void EncodeBin(std::vector<uint8_t> &binData, std::vector<uint8_t> &encodedData);
    void EncodeVLQ(uint64_t vlqData, std::vector<uint8_t> &encodedData);
    void Display(std::vector<uint8_t> &data);
    void Display(MibObject* obj);
public:
    KoBER(MibObject *root);
    std::vector<uint8_t> &GetBerData(void) {return berData;};
};