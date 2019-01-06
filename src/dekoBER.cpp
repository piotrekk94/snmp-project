#include <dekoBER.hpp>
#include <object.hpp>
#include <cstdio>

static const char* const classNames[] = {"UNIVERSAL", "APPLICATION", "CONTEXT-SPECIFIC", "PRIVATE"};
static const char* const typeNames[] = {"PRIMITIVE", "CONSTRUCTED"};

static BerTag tagNames;

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

void DekoBER::Decode(std::vector<uint8_t> &encodedData, BerObject *parent)
{
    std::size_t currentOctet = 0;

    /* process all objects in encodedData */
    while(currentOctet < encodedData.size()){
        uint8_t objClass;
        uint8_t objType;
        uint64_t objTag;
        uint64_t objLen;
        std::vector<uint8_t> objData;
        Type *objMIBType = nullptr;

        objClass = (encodedData[currentOctet] >> 6) & 0x03;
        objType = (encodedData[currentOctet] >> 5) & 0x01;
        objTag = encodedData[currentOctet] & 0x1F;

        /* multi octet tag */
        if(objTag == 0x1F){
            std::vector<uint8_t> tagOctets;
            while((encodedData[++currentOctet] & 0x80) != 0){
                tagOctets.push_back(encodedData[currentOctet] & 0x7F);
            }

            objTag = encodedData[currentOctet];

            for(int i = 1; !tagOctets.empty(); i++){
                uint64_t tmp = tagOctets.back();
                tagOctets.pop_back();
                for(int j = 0; j < i; j++)
                    tmp = tmp << 7;

                objTag += tmp;
            }
        }

        objLen = encodedData[++currentOctet];

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
        if(objLen == 0){
            for(std::size_t i = 0; i < objLen; i++){
                objData.push_back(encodedData[++currentOctet]);
                if(i > 0 && encodedData[currentOctet] == 0 && encodedData[currentOctet - 1] == 0){
                    objData.pop_back();
                    objData.pop_back();
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