#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>
#include <iostream>
#include <string>
#include <vector>

ObjectTree *tree;

int main(int argc, char* argv[]){
	parser p;
	std::vector<std::string> imports;

	Object iso("iso");
	tree = new ObjectTree(&iso);

	if(argc > 1)
		p.load(argv[1], imports);
	else
		p.load("test.txt", imports);

	std::cout<<"Tree structure:"<<std::endl<<*tree;

	return 0;
}
