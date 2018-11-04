#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]){
	parser p;

	Object iso("iso");
	tree = new ObjectTree(&iso);

	if(argc > 1)
		p.load(argv[1]);
	else
		p.load("test.txt");

	std::cout<<"Tree structure:"<<std::endl<<*tree;

	std::cout<<"Types:"<<std::endl;

	for(auto &val : types)
		std::cout<<val<<std::endl;

	return 0;
}
