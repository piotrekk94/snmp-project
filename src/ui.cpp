#include <dekoBER.hpp>
#include <koBER.hpp>
#include <algorithm>
#include <iostream>
#include <PDU.hpp>
#include <string>

static void str2hex(std::string &str, std::vector<uint8_t> &hex)
{
    str.erase(remove_if(str.begin(), str.end(), isspace), str.end());

    for(int i = 0; i < str.length(); i += 2){
        std::string tmp;
        tmp += str[i];
        tmp += str[i + 1];
        hex.push_back(std::stoi(tmp, nullptr, 16));
    }
}

void test_dekober(void)
{

    printf("*******\nDEKOBER\n*******\n");

    std::vector<uint8_t> berStr {0x04, 0x04, 0x01, 0x02, 0x03, 0x04};

    DekoBER decStr(berStr);

    std::vector<uint8_t> berInt {0x02, 0x02, 0xFF, 0x7F};

    DekoBER decInt(berInt);

    std::vector<uint8_t> ber2Int {0x30, 0x06, 0x02, 0x01, 0x03, 0x02, 0x01, 0x08};

    DekoBER dec2Int(ber2Int);

    std::vector<uint8_t> berSeq2Int {0x30, 0x08, 0x30, 0x06, 0x02, 0x01, 0x03, 0x02, 0x01, 0x08};

    DekoBER decSeq2Int(berSeq2Int);

    std::vector<uint8_t> berMIB {0x30, 0x08, 0x30, 0x06, 0x42, 0x01, 0x03, 0x41, 0x01, 0x08};

    DekoBER decMIB(berMIB);

    std::vector<uint8_t> berUndef {0x02, 0x00, 0x02, 0x01, 0x03, 0x07, 0x00, 0x00};

    DekoBER decUndef(berUndef);



    std::vector<uint8_t> snmpGet {
        0x30, 0x29, 0x02, 0x01, 0x00, 0x04, /* .F0).... */
        0x06, 0x70, 0x75, 0x62, 0x6c, 0x69, 0x63, 0xa0, /* .public. */
        0x1c, 0x02, 0x04, 0x22, 0x75, 0xbd, 0x27, 0x02, /* ..."u.'. */
        0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x0e, 0x30, /* .....0.0 */
        0x0c, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x02, 0x01, /* ...+.... */
        0x01, 0x05, 0x00, 0x05, 0x00                    /* ..... */
        };

    DekoBER decSnmp(snmpGet);

    printf("-------\nDEKOBER\n-------\n");
}

void test_kober(void)
{
    printf("*****\nKOBER\n*****\n");

    MibObject mibInt(0, 0, 2);
    mibInt.setIntData(0x2137);

    KoBER encInt(&mibInt);

    MibObject mibStr(0, 0, 4);
    mibStr.setBinData({'A', 'B', 'C', 'D'});

    KoBER encStr(&mibStr);

    MibObject mibNul(0, 0, 5);

    KoBER encNul(&mibNul);

    MibObject mibOid(0, 0, 6);
    mibOid.setOidData({1, 3, 350, 212, 357});

    KoBER encOid(&mibOid);

    MibObject mibSeq(0, 1, 16);
    mibSeq.AddChild(&mibInt);
    mibSeq.AddChild(&mibStr);
    mibSeq.AddChild(&mibNul);
    mibSeq.AddChild(&mibOid);

    KoBER encSeq(&mibSeq);

    MibObject mibSeq2(0, 1, 16);

    mibSeq2.AddChild(&mibSeq);
    mibSeq2.AddChild(&mibStr);

    KoBER encSeq2(&mibSeq2);

    printf("-----\nKOBER\n-----\n");

    printf("*=+-\nFINAL FORM\n-+=*\n");

    DekoBER finalForm(encSeq2.GetBerData());
}

void test_pdu(void)
{
    int resp = 0;

    MibObject mibNul(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::NUL);
    MibObject mibOid(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::OID);
    mibOid.setOidData({1, 3, 350, 212, 357});
    MibObject mibSeq(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);
    mibSeq.AddChild(&mibOid);
    mibSeq.AddChild(&mibNul);

    std::vector<MibObject*> varBindList {&mibSeq};

    PDU enc(GetRequest, 1977690072LLU, 0LLU, 0LLU, varBindList);

    for(;;){
        std::string str;
        std::vector<uint8_t> hex;

        printf("Paste packet in hex to decode:\n");
        std::getline(std::cin, str);

        str2hex(str, hex);

        PDU pdu(hex);

        if(pdu.pktType == GetRequest){
            printf("PDU RESPONSE GET:\n");

            MibObject mibIntR(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
            mibIntR.setIntData(resp++);
            MibObject mibOidR(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::OID);
            std::vector<uint64_t> oid;
            pdu.varBind->children[0]->children[0]->AsOid(oid);
            mibOidR.setOidData(oid);
            MibObject mibSeqR(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);
            mibSeqR.AddChild(&mibOidR);
            mibSeqR.AddChild(&mibIntR);

            std::vector<MibObject*> varBindListResp {&mibSeqR};

            PDU enc(GetResponse, pdu.requestID, 0LLU, 0LLU, varBindListResp);
        }

        if(pdu.pktType == SetRequest){
            printf("PDU RESPONSE SET:\n");

            resp = pdu.varBind->children[0]->children[1]->AsInt();

            MibObject mibIntR(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::INT);
            mibIntR.setIntData(resp);
            MibObject mibOidR(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::OID);
            std::vector<uint64_t> oid;
            pdu.varBind->children[0]->children[0]->AsOid(oid);
            mibOidR.setOidData(oid);
            MibObject mibSeqR(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);
            mibSeqR.AddChild(&mibOidR);
            mibSeqR.AddChild(&mibIntR);

            std::vector<MibObject*> varBindListResp {&mibSeqR};

            PDU enc(GetResponse, pdu.requestID, 0LLU, 0LLU, varBindListResp);
        }
    }
}