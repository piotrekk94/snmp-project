#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>
#include <iostream>
#include <string>
#include <vector>

ObjectTree *tree;

int main(int argc, char* argv[]){
	parser p;

	Object iso("iso");
	tree = new ObjectTree(&iso);

	if(argc > 1)
		p.load(argv[1]);
	else
		p.load("test.txt");

	std::cout<<"Tree structure:"<<std::endl<<*tree;

	return 0;
}
