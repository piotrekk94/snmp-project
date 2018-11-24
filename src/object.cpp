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
    this->constraints = constraints;
    this->sequence = sequence;
    if(constraints.length() > 0)
        this->constr = true;
    else
        this->constr = false;
    
    if(visibility.length() > 0)
        this->vis = true;
    else
        this->vis = false;

    if(sequence.size() > 0)
        this->seq = true;
    else
        this->seq = false;
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
std::string Type::getConstraints(){
    return this->constraints;
}

std::vector<seq_type> Type::getSequence(){
    return this->sequence;
}

bool Type::hasConstraints(){
    return this->constr;
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
    if(type.hasConstraints())
        os<<type.getConstraints()<<std::endl;
    if(type.hasSequence()){
        auto seq = type.getSequence();
        for(const auto& val : seq)
            os<<val.name<<" : "<<val.type<<"["<<val.cons<<"]"<<std::endl;
    }
    return os;
}