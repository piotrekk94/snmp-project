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
    bool implicit;
public:
    Type(std::string name, std::string visibility, std::string mode, std::string typeName, std::string constraints, std::vector<seq_type> &sequence);
    std::string getName();
    int getVisibility();
    int getTypeId();
    bool getImplicit();
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
    std::vector<unsigned int> getConstraints();
};

struct ObjectPath {
    std::vector<std::tuple<std::string, int>> path;
    ObjectPath(){
        
    }
    ObjectPath(std::vector<int> path){
        this->path.push_back(std::make_tuple("iso", 1));
        for(int i = 1; i < path.size(); i++){
		    this->path.push_back(std::make_tuple("", path[i]));
	}
    }
};