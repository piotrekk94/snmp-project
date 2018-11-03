#include <object.hpp>

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