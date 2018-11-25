#include <object.hpp>
#include <iostream>

Object::Object(std::string name){
    this->name = name;
    this->leaf = false;
}

Object::Object(std::string name, std::string syntax, std::string access, std::string status, std::string desc){
    this->name = name;
    this->syntax = syntax;
    this->access = access;
    this->status = status;
    this->desc = desc;
    this->leaf = true;
}

std::string Object::getName(){
    return this->name;
}

bool Object::isLeaf(){
    return this->leaf;
}

std::string Object::getSyntax(){
    return this->syntax;
}
std::string Object::getAccess(){
    return this->access;
}
std::string Object::getStatus(){
    return this->status;
}
std::string Object::getDesc(){
    return this->desc;
}

std::vector<Type> types;

Type::Type(std::string name, std::string visibility, std::string typeName, std::string constraints, std::vector<seq_type> &sequence){
    this->name = name;
    this->visibility = visibility;
    this->typeName = typeName;
    this->sequence = sequence;
    
    if(visibility.length() > 0)
        this->vis = true;
    else
        this->vis = false;

    if(sequence.size() > 0)
        this->seq = true;
    else
        this->seq = false;

    std::string::const_iterator iter = constraints.begin();
    std::string::const_iterator end = constraints.end();

    std::vector<unsigned int> constr;

    namespace qi = boost::spirit::qi;
	bool r = qi::parse(iter, end, qi::uint_ % qi::lit(".."), constr);
    this->constraints = constr;

}

std::string Type::getName(){
    return this->name;
}
std::string Type::getVisibility(){
    return this->visibility;
}
std::string Type::getTypeName(){
    return this->typeName;
}
std::vector<unsigned int> Type::getConstraints(){
    return this->constraints;
}

std::vector<seq_type> Type::getSequence(){
    return this->sequence;
}

bool Type::hasConstraints(){
    return this->constraints.size() > 0;
}

bool Type::hasVisibility(){
    return this->vis;
}

bool Type::hasSequence(){
    return this->seq;
}

std::ostream &operator<<(std::ostream &os, Type &type){
    os<<type.getName()<<std::endl;
    if(type.hasVisibility())
        os<<type.getVisibility()<<std::endl;
    os<<type.getTypeName()<<std::endl;
    if(type.hasConstraints()){
        auto constr = type.getConstraints();
        if(constr.size() == 1)
            os<<constr[0]<<std::endl;
        else
            os<<constr[0]<<":"<<constr[1]<<std::endl;
    }
    if(type.hasSequence()){
        auto seq = type.getSequence();
        for(const auto& val : seq){
            os<<val.name<<" : "<<val.type<<" ";
            if(val.cons.size() == 1){
                os<<val.cons[0];
            } else if(val.cons.size() == 2){
                os<<val.cons[0]<<":"<<val.cons[1];
            }
            os<<std::endl;
        }
            
    }
    return os;
}