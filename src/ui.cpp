#include <dekoBER.hpp>
#include <koBER.hpp>

void test_dekober(void)
{
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
    mibOid.setOidData({1, 3, 350, 212});

    KoBER encOid(&mibOid);

    MibObject mibSeq(0, 1, 16);
    mibSeq.AddChild(&mibInt);
    mibSeq.AddChild(&mibStr);
    mibSeq.AddChild(&mibNul);
    mibSeq.AddChild(&mibOid);

    KoBER encSeq(&mibSeq);

    printf("-----\nKOBER\n-----\n");
}