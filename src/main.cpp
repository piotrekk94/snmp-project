#include <parser.hpp>
#include <string>
#include <vector>

int main(int argc, char* argv[]){
	parser p;
	std::vector<std::string> imports;
	
	if(argc > 1)
		p.load(argv[1], imports);

	return 0;
}
