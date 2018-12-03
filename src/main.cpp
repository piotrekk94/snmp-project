#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ui.hpp>

void types_init(void){
	std::vector<seq_type> seq;

	Type INT("INTEGER", "UNIVERSAL 2", "IMPLICIT", "INTEGER", "", seq);
	Type OCT("OCTET STRING", "UNIVERSAL 4", "IMPLICIT", "OCTET STRING", "", seq);
	Type NUL("NULL", "UNIVERSAL 5", "IMPLICIT", "NULL", "", seq);
	Type OID("OBJECT IDENTIFIER", "UNIVERSAL 6", "IMPLICIT", "OBJECT IDENTIFIER", "", seq);

	types.push_back(INT);
	types.push_back(OCT);
	types.push_back(NUL);
	types.push_back(OID);
}

int main(int argc, char* argv[]){

	Object iso("iso");
	tree = new ObjectTree(&iso);

	types_init();

	if(argc > 1)
		p.load(argv[1]);
	else
		p.load("test.txt");

	std::cout<<"Tree structure:"<<std::endl<<*tree;

	std::cout<<"Types:"<<std::endl;

	for(auto &val : types)
		std::cout<<val<<std::endl;

	for(;;){
		test_ber();
	}

	return 0;
}
