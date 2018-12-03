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