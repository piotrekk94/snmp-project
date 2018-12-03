#include <ber.hpp>
#include <stack>

static inline Type& getType(Object *object){
    for (auto &type : types)
        if(type.getName() == object->getTypeName())
            return type;
}

namespace qi = boost::spirit::qi;

void Encoder::encodeSize(int size){
    if(size < 128){
        this->encoded.push_back(size);
    } else {
        throw "Unimplemented";
    }
}

void Encoder::encodeEOC(void){
    encoded.push_back(0);
    encoded.push_back(0);
}

void Encoder::encodeOID(int oid){
    std::stack<unsigned char> stack;

    stack.push(oid % 128);
    oid /= 128;

    while(oid > 0){
        stack.push(0x80 | oid % 128);
        oid /= 128;
    }

    for(stack; !stack.empty(); stack.pop())
        encoded.push_back(stack.top());
}

void Encoder::encodeInt(long long value){
    int bytes;

    for(bytes = 1; bytes <= 8; bytes++)
        if(value < 0){
            if(-value <= (pow(2, (bytes * 8) - 1)))
                break;
        } else {
            if(value <= (pow(2, (bytes * 8) - 1) - 1))
                break;
        }

    encodeSize(bytes);
    for(bytes; bytes > 0; bytes--)
        encoded.push_back(value >> ((bytes - 1) * 8));
}

void Encoder::encodeValue(int type, std::string value){
    switch(type){
        case INTEGER:
        encodeInt(stoll(value));
        break;
        case OCTET:
        encodeSize(value.length());
        for(auto &c : value)
            encoded.push_back(c);
        break;
        case NUL:
        encodeSize(0);
        break;
        case OID:
        std::vector<int> oid;
	    qi::parse(value.begin(), value.end(), qi::int_ % '.', oid);
        encodeSize(oid.size() - 1);
        encoded.push_back(oid[0] * 40 + oid[1]);
        for(int i = 2; i < oid.size(); i++)
            encodeOID(oid[i]);
        break;
    }
}

void Encoder::encodeTag(int cls, bool primitive, int n){
    if(n < 30){
        unsigned char tag = 0;
        tag |= (cls & 0x03) << 6;
        if(!primitive)
            tag |= 0x01 << 5;
        tag |= n & 0x1F;
        encoded.push_back(tag);
    }else{
        throw "Tags over 30 are unsupported";
    }
}

Encoder::Encoder(Object *object, std::string value){
    this->obj = object;
    this->val = value;
}

void Encoder::encode(void){

    auto type = getType(obj);

    auto typeName = type.getTypeName();

    if(!type.getImplicit())
        throw "Explicit tagging is unsupported";

    std::vector<unsigned int> constr = obj->getConstraints();
    if(constr.size() == 0)
        constr = type.getConstraints();

    if(typeName == "INTEGER"){
        if(constr.size() == 2){
            long long value = stoll(val);
            if(value < constr[0] || value > constr[1]){
                printf("Integer out of range, should be between %d and %d\n", constr[0], constr[1]);
                throw "Invalid data";
            }
        }
        encodeTag(type.getVisibility(), true, type.getTypeId());
        encodeValue(INTEGER, val);
    }else if(typeName == "OCTET STRING"){
        if(constr.size() == 1){
            if(val.length() > constr[0]){
                printf("String too long, should be shorter than %d\n", constr[0]);
                throw "Invalid data";
            }
        }
        encodeTag(type.getVisibility(), true, type.getTypeId());
        encodeValue(OCTET, val);
    }else if(typeName == "NULL"){
        encodeTag(type.getVisibility(), true, type.getTypeId());
        encodeValue(NUL, "");
    }else if(typeName == "OBJECT IDENTIFIER"){
        encodeTag(type.getVisibility(), true, type.getTypeId());
        encodeValue(OID, val);
    }

    printf("Encoded data:\n");
    for(auto &val : encoded)
        printf("%02x ", val);
    printf("\n");
/*
     unsigned char tag = type.getVisibility() << 6 | type.getTypeId() & 0x1F;

            int size = 0;

            if(cons.size() == 2)
                for(size = 0; pow(2, size) < cons[1]; size++);

            size++;

            unsigned char len = size & 0x7F;

            encoded.push_back(tag);
            encoded.push_back(len);

            std::stack<unsigned char> stack;

            auto tmp = value;

            for(int i = 0; i < len / 8; i++){
                stack.push(tmp & 0xFF); 
                tmp >>= 8;
            }

            while(!stack.empty()){
                encoded.push_back(stack.top());
                stack.pop();
            }
                
            printf("Encoded value:\n");

            for(auto &val : encoded)
                printf("%02x ", val);

            printf("\n");
*/
}