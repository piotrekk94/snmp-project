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