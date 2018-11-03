#pragma once
#include <string>
#include <vector>
#include <tuple>

class Object {
private:
    bool leaf;
    std::string name;
    std::string syntax;
    std::string access;
    std::string status;
    std::string desc;
public:
    Object(std::string name);
    Object(std::string name, std::string syntax, std::string access, std::string status, std::string desc);
    std::string getName();
    bool isLeaf();
    std::string getSyntax();
    std::string getAccess();
    std::string getStatus();
    std::string getDesc();
};

struct ObjectPath {
    std::vector<std::tuple<std::string, int>> path;
};