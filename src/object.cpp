#include <object.hpp>
#include <iostream>

Object::Object(std::string name){
    this->name = name;
    this->leaf = false;
}

Object::Object(std::string name, std::string typeName, std::string constraints, std::vector<seq_type> &sequence, std::string access, std::string status, std::string desc){
    this->name = name;
    this->typeName = typeName;
    this->sequence = sequence;
    this->access = access;
    this->status = status;
    this->desc = desc;
    this->leaf = true;

    if(constraints.length() > 0){
        std::string::const_iterator iter = constraints.begin();
        std::string::const_iterator end = constraints.end();

        std::vector<unsigned int> constr;

        namespace qi = boost::spirit::qi;
        qi::parse(iter, end, qi::uint_ % qi::lit(".."), constr);
        this->constraints = constr;
    }

}

std::string Object::getName(){
    return this->name;
}

bool Object::isLeaf(){
    return this->leaf;
}

std::string Object::getTypeName(){
    return this->typeName;
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
    //this->visibility = visibility;
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


    if(constraints.length() > 0){
        std::string::const_iterator iter = constraints.begin();
        std::string::const_iterator end = constraints.end();

        std::vector<unsigned int> constr;

        namespace qi = boost::spirit::qi;
        qi::parse(iter, end, qi::uint_ % qi::lit(".."), constr);
        this->constraints = constr;
    }

    this->visibility = CONTEXT_SPECIFIC;
    this->typeId = 2137;

    if(visibility.length() > 0){
        std::string::const_iterator iter = visibility.begin();
        std::string::const_iterator end = visibility.end();

        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;
        using boost::phoenix::ref;
        qi::phrase_parse(iter, end, (qi::lit("UNIVERSAL")[ref(this->visibility) = UNIVERSAL] | qi::lit("APPLICATION")[ref(this->visibility) = APPLICATION] | qi::lit("CONTEXT-SPECIFIC")[ref(this->visibility) = CONTEXT_SPECIFIC] | qi::lit("PRIVATE")[ref(this->visibility) = PRIVATE]) >> qi::int_[ref(this->typeId) = qi::_1], qi::char_(' '));
    }

}

std::string Type::getName(){
    return this->name;
}
int Type::getVisibility(){
    return this->visibility;
}
int Type::getTypeId(){
    return this->typeId;
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

bool Type::hasSequence(){
    return this->seq;
}

std::ostream &operator<<(std::ostream &os, Type &type){
    os<<type.getName()<<std::endl;
    os<<type.getTypeName()<<std::endl;
    os<<VisibilityNames[type.getVisibility()]<<std::endl;
    os<<type.getTypeId()<<std::endl;
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