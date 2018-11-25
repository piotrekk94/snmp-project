#include <ui.hpp>

namespace qi = boost::spirit::qi;

void test_ber(void){
    std::string oid;
    int value;
    std::cout<<"OID:"<<std::endl;
    std::cin>>oid;
    std::cout<<"Value:"<<std::endl;
    std::cin>>value;

	std::string::const_iterator iter = oid.begin();
    std::string::const_iterator end = oid.end();

    std::vector<int> oid_v;

	bool r = qi::parse(iter, end, qi::int_ % '.', oid_v);

    ObjectPath path;

    path.path.push_back(std::make_tuple("iso", 1));
	for(int i = 1; i < oid_v.size(); i++){
		path.path.push_back(std::make_tuple("", oid_v[i]));
	}

    auto node = tree->getObject(path);

    if(node != nullptr)
        std::cout<<node->getName()<<std::endl<<node->getSyntax()<<std::endl;

}