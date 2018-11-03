#include <object.hpp>

Object::Object(std::string name){
    this->name = name;
}

Object::Object(std::string name, std::string syntax, std::string access, std::string status, std::string desc){
    this->name = name;
    this->syntax = syntax;
    this->access = access;
    this->status = status;
    this->desc = desc;
}

std::string Object::getName(){
    return this->name;
}