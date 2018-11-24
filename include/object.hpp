#pragma once
#include <string>
#include <vector>
#include <tuple>

struct seq_type {
    std::string name;
    std::string type;
    std::string cons;
};

class Type {
private:
    std::string name;
    std::string visibility;
    std::string typeName;
    std::string constraints;
    std::vector<seq_type> sequence;
    bool constr;
    bool vis;
    bool seq;
public:
    Type(std::string name, std::string visibility, std::string typeName, std::string constraints, std::vector<seq_type> &sequence);
    std::string getName();
    std::string getVisibility();
    std::string getTypeName();
    std::string getConstraints();
    std::vector<seq_type> getSequence();
    bool hasConstraints();
    bool hasVisibility();
    bool hasSequence();
};

std::ostream &operator<<(std::ostream &os, Type &type);

extern std::vector<Type> types;

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