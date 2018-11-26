#pragma once
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <string>
#include <string>
#include <vector>
#include <tuple>

enum Visibility {UNIVERSAL, APPLICATION, CONTEXT_SPECIFIC, PRIVATE};

const std::string VisibilityNames[] = {"UNIVERSAL", "APPLICATION", "CONTEXT-SPECIFIC", "PRIVATE"};

struct seq_type {
    std::string name;
    std::string type;
    std::vector<unsigned int> cons;
};

class Type {
private:
    std::string name;
    int visibility;
    int typeId;
    std::string typeName;
    std::vector<unsigned int> constraints;
    std::vector<seq_type> sequence;
    bool vis;
    bool seq;
public:
    Type(std::string name, std::string visibility, std::string typeName, std::string constraints, std::vector<seq_type> &sequence);
    std::string getName();
    int getVisibility();
    int getTypeId();
    std::string getTypeName();
    std::vector<unsigned int> getConstraints();
    std::vector<seq_type> getSequence();
    bool hasConstraints();
    bool hasSequence();
};

std::ostream &operator<<(std::ostream &os, Type &type);

extern std::vector<Type> types;

class Object {
private:
    bool leaf;
    std::string name;
    std::string typeName;
    std::vector<unsigned int> constraints;
    std::vector<seq_type> sequence;
    std::string access;
    std::string status;
    std::string desc;
public:
    Object(std::string name);
    Object(std::string name, std::string typeName, std::string constraints, std::vector<seq_type> &sequence, std::string access, std::string status, std::string desc);
    std::string getName();
    bool isLeaf();
    std::string getTypeName();
    std::string getAccess();
    std::string getStatus();
    std::string getDesc();
};

struct ObjectPath {
    std::vector<std::tuple<std::string, int>> path;
};