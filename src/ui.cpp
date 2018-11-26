#include <ui.hpp>
#include <stack>

namespace qi = boost::spirit::qi;

void test_ber(void){
    std::string oid;
    long long value;
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
        std::cout<<node->getName()<<std::endl<<node->getTypeName()<<std::endl;

    for(auto &type : types){
        if(type.getName() == node->getTypeName()){
            std::cout<<"Found data type"<<std::endl<<type<<std::endl;

            auto cons = type.getConstraints();

            if(cons.size() == 2)
                if((value < (long long)cons[0]) || (value > (long long)cons[1])){
                    std::cout<<"Value out of range: "<<value<<std::endl;
                    std::cout<<"Should be between "<<cons[0]<<"=<VALUE=<"<<cons[1]<<std::endl;
                }

            std::vector<unsigned char> encoded;

            unsigned char tag = type.getVisibility() << 6 | type.getTypeId() & 0x1F;

            int size = 0;

            if(cons.size() == 2)
                for(size = 0; pow(2, size) < cons[1]; size++);

            size++;

            unsigned char len = size & 0x7F;

            encoded.push_back(tag);
            encoded.push_back(len);

            std::stack<unsigned char> stack;

            auto tmp = value;

            for(int i = 0; i < len / 8; i++){
                stack.push(tmp & 0xFF); 
                tmp >>= 8;
            }

            while(!stack.empty()){
                encoded.push_back(stack.top());
                stack.pop();
            }
                
            printf("Encoded value:\n");

            for(auto &val : encoded)
                printf("%02x ", val);

            printf("\n");

            break;
        }
    }

}