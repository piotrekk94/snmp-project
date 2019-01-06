#include <dekoBER.hpp>

#if 0
#include <ui.hpp>
#include <stack>
#include <ber.hpp>

namespace qi = boost::spirit::qi;

void test_ber(void){
    std::string oid;
    std::string value;
    std::cout<<"OID:"<<std::endl;
    std::getline(std::cin, oid);
    std::cout<<"Value:"<<std::endl;
    std::getline(std::cin, value);

	std::string::const_iterator iter = oid.begin();
    std::string::const_iterator end = oid.end();

    std::vector<int> oid_v;

	qi::parse(iter, end, qi::int_ % '.', oid_v);

    ObjectPath path(oid_v);

    auto node = tree->getObject(path);

    if(node != nullptr){
        std::cout<<"Entry found: "<<node->getName()<<" : "<<node->getTypeName()<<std::endl;
    }

    Encoder enc(node, value);

    enc.encode();

}

#endif

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
}