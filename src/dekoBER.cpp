#include <dekoBER.hpp>
#include <object.hpp>
#include <cstdio>
#include <cctype>

static const char* const classNames[] = {"UNIVERSAL", "APPLICATION", "CONTEXT-SPECIFIC", "PRIVATE"};
static const char* const typeNames[] = {"PRIMITIVE", "CONSTRUCTED"};

static BerTag tagNames;

void BerObject::AsInt(int depth)
{
    int64_t res = 0;
    int i;

    for(i = 0; i < objData.size(); i++){
        res = (res << 8) | objData[i];
    }

    res = res << (8 - i) * 8;

    res = res >> (8 - i) * 8;

    printf("%*sAS INTEGER: %lld\n", depth, "", res);
}

void BerObject::AsOid(int depth)
{
    std::vector<uint64_t> oidData;
    std::size_t currentOctet = 0;
    uint8_t x, y;

    x = objData[0] / 40;
    y = objData[0] % 40;

    oidData.push_back(x);
    oidData.push_back(y);

    while(currentOctet < objData.size() - 1){
        oidData.push_back(DecodeVLQ(currentOctet, objData));
    }

    printf("%*sAS OID: ", depth, "");

    for(auto &val : oidData)
        printf(".%llu", val);

    printf("\n");
}

void BerObject::AsStr(int depth)
{
    printf("%*sAS STR: ", depth, "");

    for(auto &val : objData)
        printf("%c", isalnum(val) ? (char)val : '.');

    printf("\n");
}

uint64_t BerObject::DecodeVLQ(std::size_t &currentOctet, std::vector<uint8_t> &encodedData)
{
    uint64_t res;
    std::vector<uint8_t> vlqOctets;

    while((encodedData[++currentOctet] & 0x80) != 0 && currentOctet < encodedData.size()){
        vlqOctets.push_back(encodedData[currentOctet] & 0x7F);
    }

    res = encodedData[currentOctet];

    for(int i = 1; !vlqOctets.empty(); i++){
        uint64_t tmp = vlqOctets.back();
        vlqOctets.pop_back();
        for(int j = 0; j < i; j++)
            tmp = tmp << 7;

        res += tmp;
    }
    
    return res;
}

void BerObject::Display(int depth)
{
    printf("%*s----\n", depth, "");
    printf("%*sCLASS: %s(%d)\n", depth, "", classNames[objClass], objClass);
    printf("%*sTYPE: %s(%d)\n", depth, "", typeNames[objType], objType);
    if(objClass == UNIVERSAL){
        printf("%*sTAG: %s(%llu)\n", depth, "", tagNames[objTag], objTag);
    } else {
        printf("%*sTAG: OTHER(%llu)\n", depth, "", objTag);
    }
    printf("%*sLENGTH: %llu\n", depth, "", objLen);
    printf("%*sDATA:", depth, "");
    for(int i = 0; i < objData.size(); i++){
        if((i % 16) == 0){
            printf("\n%*s", depth, "");
        }
        printf("%02x ", objData[i]);
    }

    printf("\n");

    if(objClass == UNIVERSAL && objTag == 2){
        AsInt(depth);
    }
    
    if(objClass == UNIVERSAL && objTag == 4){
        AsStr(depth);
    }

    if(objClass == UNIVERSAL && objTag == 6){
        AsOid(depth);
    }

    if(objMIBType == nullptr){
        printf("%*sNO MIB TYPE FOUND\n", depth, "");
    } else {
        printf("%*sMIB TYPE FOUND: %s\n", depth, "", objMIBType->getName().c_str());
    }

    for(int i = 0; i < children.size(); i++){
        children[i]->Display(depth + 4);
    }
}

void DekoBER::Display(std::vector<uint8_t> &data)
{
    for(int i = 0; i < data.size(); i++){
        if((i % 16) == 0){
            printf("\n");
        }
        printf("%02x ", data[i]);
    }
    printf("\n");
}

void DekoBER::Display(BerObject *obj)
{
    obj->Display(0);
}

DekoBER::DekoBER(std::vector<uint8_t> &encodedData)
{
    printf("==*START*==\n");
    printf("Decoding binary data:");
    Display(encodedData);
    Decode(encodedData, nullptr);
    printf("\nDecoded object structure:\n");
    Display(root);
}

uint64_t DekoBER::DecodeVLQ(std::size_t &currentOctet, std::vector<uint8_t> &encodedData)
{
    uint64_t res;
    std::vector<uint8_t> vlqOctets;

    while((encodedData[++currentOctet] & 0x80) != 0){
        vlqOctets.push_back(encodedData[currentOctet] & 0x7F);
    }

    res = encodedData[currentOctet];

    for(int i = 1; !vlqOctets.empty(); i++){
        uint64_t tmp = vlqOctets.back();
        vlqOctets.pop_back();
        for(int j = 0; j < i; j++)
            tmp = tmp << 7;

        res += tmp;
    }
    
    return res;
}

void DekoBER::Decode(std::vector<uint8_t> &encodedData, BerObject *parent)
{
    std::size_t currentOctet = 0;

    /* process all objects in encodedData */
    while(currentOctet < encodedData.size()){
        uint8_t objClass;
        uint8_t objType;
        uint64_t objTag;
        uint64_t objLen;
        bool undefinedLength;
        std::vector<uint8_t> objData;
        Type *objMIBType = nullptr;

        objClass = (encodedData[currentOctet] >> 6) & 0x03;
        objType = (encodedData[currentOctet] >> 5) & 0x01;
        objTag = encodedData[currentOctet] & 0x1F;

        /* multi octet tag */
        if(objTag == 0x1F){
            objTag = DecodeVLQ(currentOctet, encodedData);
        }

        objLen = encodedData[++currentOctet];

        if(objLen == 0x80){
            undefinedLength = true;
        }else{
            undefinedLength = false;
        }

        /* multi octet length */
        if((objLen & 0x80) != 0){
            std::vector<uint8_t> lengthOctets;
            int lengthOctetsCount = objLen & 0x7F;

            for(int i = 0; i < lengthOctetsCount; i++){
                lengthOctets.push_back(encodedData[++currentOctet]);
            }

            objLen = 0;

            for(int i = 0; !lengthOctets.empty(); i++){
                uint64_t tmp = lengthOctets.back();
                lengthOctets.pop_back();
                for(int j = 0; j < i; j++)
                    tmp = tmp << 8;

                objLen += tmp;
            }
        }

        /* undefined length */
        if(undefinedLength){
            for(std::size_t i = 0; (currentOctet + 1) < encodedData.size(); i++){
                objData.push_back(encodedData[++currentOctet]);
                if(i > 0 && encodedData[currentOctet] == 0 && encodedData[currentOctet - 1] == 0){
                    objData.pop_back();
                    objData.pop_back();
                    break;
                }
            }
        } else {
            for(std::size_t i = 0; i < objLen; i++){
                objData.push_back(encodedData[++currentOctet]);
            }
        }


        if(objClass == APPLICATION){
            for(auto &val : types){
                if(val.getVisibility() == APPLICATION){
                    if(val.getTypeId() == objTag){
                        objMIBType = &val;
                    }
                }
            }
        }

        BerObject *obj = new BerObject(objClass, objType, objTag, objMIBType, objLen, objData);

        if(parent == nullptr){
            root = obj;
        } else {
            parent->AddChild(obj);
        }

        /* constructed type contains more objects inside */
        if(objType == 1){
            Decode(objData, obj);
        }

        /* mode to first octet after encoded object */
        currentOctet++;
    }
}