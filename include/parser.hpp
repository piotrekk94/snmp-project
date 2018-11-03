#pragma once
#include <string>
#include <vector>

class parser {
public:
	bool load(std::string filename);
	bool load(std::string filename, std::vector<std::string> imports);
};
