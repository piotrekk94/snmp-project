#include "PDU.hpp"
#include <cstdio>
#include "tree.hpp"

void PDU::Encode(std::vector<MibObject*> &varBindList)
{
    MibObject varBind(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);
    for(auto &val : varBindList){
        varBind.AddChild(val);
    }

    MibObject errIdx(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
    errIdx.setIntData(errorIndex);

    MibObject errSta(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
    errSta.setIntData(errorStatus);

    MibObject reqID(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
    reqID.setIntData(requestID);

    MibObject data(PDUEnum::CTX, PDUEnum::CON, pktType);

    data.AddChild(&reqID);
    data.AddChild(&errSta);
    data.AddChild(&errIdx);
    data.AddChild(&varBind);

    MibObject community(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::OCT);
    community.setBinData({'p', 'u', 'b', 'l', 'i', 'c'});

    MibObject version(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
    version.setIntData(0);

    MibObject snmpPkt(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);

    snmpPkt.AddChild(&version);
    snmpPkt.AddChild(&community);
    snmpPkt.AddChild(&data);

    KoBER pkt(&snmpPkt);

    auto &pktData = pkt.GetBerData();

    encodedPkt.assign(pktData.begin(), pktData.end());

    printf("Encoded PDU: \n");
    for(auto &val : encodedPkt)
        printf("%02x ", val);

    printf("\n");

}

void PDU::Decode(void)
{
    DekoBER pkt(encodedPkt);
    root = pkt.GetBerTree();

    pktType = (PDUType)root->children[2]->GetTag();
    /*      **snmpPkt****data*******reqID**         */
    requestID = root->children[2]->children[0]->AsInt();
    errorStatus = root->children[2]->children[1]->AsInt();
    errorIndex = root->children[2]->children[2]->AsInt();

    varBind = root->children[2]->children[3];

    printf("PDU DECODE\n");
    printf("PKT TYPE: %d\n", pktType);
    printf("REQ ID: %llu\n", requestID);
    printf("ERR STA: %llu\n", errorStatus);
    printf("ERR IDX: %llu\n", errorIndex);
    printf("VAR BIND:\n");

    int i = 1;
    for(auto &val : varBind->children){
        printf("Entry %d:\n", i++);
        printf("OID:\n");
        val->children[0]->Display(4);
        printf("DATA:\n");
        val->children[1]->Display(4);
    }
}
