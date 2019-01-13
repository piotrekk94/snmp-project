#pragma once
#include <cstdint>
#include <vector>
#include <koBER.hpp>
#include <dekoBER.hpp>

enum PDUType {
    GetRequest = 0,
    GetNextRequest = 1,
    GetResponse = 2,
    SetRequest = 3,
};

namespace PDUEnum {
    enum ObjClass {
        UNI = 0,
        APP = 1,
        CTX = 2,
        PRV = 3,
    };

    enum ObjPri {
        PRM = 0,
        CON = 1,
    }; 

    enum ObjType {
        INT = 2,
        OCT = 4,
        NUL = 5,
        OID = 6,
        SEQ = 16,
    };
}

class PDU {
public:
    std::vector<uint8_t> encodedPkt;

    PDUType pktType;
    uint64_t requestID;
    uint64_t errorStatus;
    uint64_t errorIndex;

    BerObject *root;
    BerObject *varBind;

    void Encode(std::vector<MibObject*> &varBindList);
    void Decode(void);

    PDU(std::vector<uint8_t> &packet) {this->encodedPkt.assign(packet.begin(), packet.end());Decode();};
    PDU(PDUType pktType, uint64_t requestID, uint64_t errorStatus, uint64_t errorIndex, std::vector<MibObject*> &varBindList)
        : pktType(pktType)
        , requestID(requestID)
        , errorStatus(errorStatus)
        , errorIndex(errorIndex)
        {Encode(varBindList);Decode();};
    BerObject *GetVarBind(void){return varBind;};
};