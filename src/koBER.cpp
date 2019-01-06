#include <dekoBER.hpp>
#include <koBER.hpp>
#include <cmath>

static const char* const classNames[] = {"UNIVERSAL", "APPLICATION", "CONTEXT-SPECIFIC", "PRIVATE"};
static const char* const typeNames[] = {"PRIMITIVE", "CONSTRUCTED"};
static const char* const dataNames[] = {"NONE", "INTEGER", "BINARY", "OID", "NULL"};

static BerTag tagNames;

void MibObject::Display(int depth)
{
    printf("%*s----\n", depth, "");
    printf("%*sCLASS: %s(%d)\n", depth, "", classNames[objClass], objClass);
    printf("%*sTYPE: %s(%d)\n", depth, "", typeNames[objType], objType);
    if(objClass == UNIVERSAL){
        printf("%*sTAG: %s(%llu)\n", depth, "", tagNames[objTag], objTag);
    } else {
        printf("%*sTAG: OTHER(%llu)\n", depth, "", objTag);
    }
    printf("%*sDATA TYPE: %s(%d)\n", depth, "", dataNames[dataType], dataType);

    printf("\n");

    for(int i = 0; i < children.size(); i++){
        children[i]->Display(depth + 4);
    }
}

KoBER::KoBER(MibObject *root)
{
    printf("Data to encode:\n");
    Display(root);
    Encode(root, berData);
    printf("Encoded data:");
    Display(berData);
}

void KoBER::Display(std::vector<uint8_t> &data)
{
    for(int i = 0; i < data.size(); i++){
        if((i % 16) == 0){
            printf("\n");
        }
        printf("%02x ", data[i]);
    }
    printf("\n");
}

void KoBER::Display(MibObject *obj)
{
    obj->Display(0);
}

void KoBER::EncodeInt(int64_t intData, std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> tmp;

    int bytes;

    for(bytes = 1; bytes <= 8; bytes++)
        if(intData < 0){
            if(-intData <= (pow(2, (bytes * 8) - 1)))
                break;
        } else {
            if(intData <= (pow(2, (bytes * 8) - 1) - 1))
                break;
        }

    for(bytes; bytes > 0; bytes--)
        encodedData.push_back(intData >> ((bytes - 1) * 8));
}

void KoBER::EncodeVLQ(uint64_t vlqData, std::vector<uint8_t> &encodedData)
{
        std::vector<uint8_t> tmp;

        while(vlqData > 0){
            tmp.push_back(vlqData & 0x7F);
            vlqData = vlqData >> 7;
        }

        while(!tmp.empty()){
            encodedData.push_back(tmp.back() | 0x80);
            tmp.pop_back();
        }

        encodedData.back() = encodedData.back() & 0x7F;
}

void KoBER::EncodeOid(std::vector<uint64_t> &oidData, std::vector<uint8_t> &encodedData)
{
    encodedData.push_back(oidData[0] * 40 + oidData[1]);
    for(int i = 2; i < oidData.size(); i++){
        EncodeVLQ(oidData[i], encodedData);
    }
}

void KoBER::EncodeBin(std::vector<uint8_t> &binData, std::vector<uint8_t> &encodedData)
{
    encodedData.insert(encodedData.end(), binData.begin(), binData.end());
}

void KoBER::EncodePayload(MibObject *obj, std::vector<uint8_t> &encodedData)
{
    switch(obj->dataType){
        case INTEGER:
            EncodeInt(obj->intData, encodedData);
            return;
        case BINARY:
            EncodeBin(obj->binData, encodedData);
            return;
        case OID:
            EncodeOid(obj->oidData, encodedData);
            return;
        case NUL:
            /* empty payload */
            return;
        default:
            /* empty payload */
            return;
    }
}

void KoBER::Encode(MibObject *obj, std::vector<uint8_t> &encodedData)
{
    std::vector<uint8_t> payload;

    /* constructed */
    if(obj->objType == 1){
        for(int i = 0; i < obj->children.size(); i++){
            Encode(obj->children[i], payload);
        }
    }else{
        EncodePayload(obj, payload);
    }

    std::vector<uint8_t> tag;

    /* multi octet tag */
    if(obj->objTag >= 31){
        uint8_t tagOctet = ((obj->objClass & 0x03) << 6) | ((obj->objType & 0x01) << 5) | 0x1F;
        tag.push_back(tagOctet);

        uint64_t tagValue = obj->objTag;

        EncodeVLQ(tagValue, tag);
    } else {
        uint8_t tagOctet = ((obj->objClass & 0x03) << 6) | ((obj->objType & 0x01) << 5) | (obj->objTag & 0x1F);
        tag.push_back(tagOctet);
    }

    std::vector<uint8_t> length;

    uint64_t lengthValue = payload.size();

    double len = std::ceil(std::log2(lengthValue));

    /* multi octet length */
    if(lengthValue >= 128){
        std::vector<uint8_t> lengthVec;
        
        while(lengthValue > 0){
            lengthVec.push_back(lengthValue & 0xFF);
            lengthValue = lengthValue >> 8;
        }

        uint8_t lengthCount = (lengthVec.size() & 0x7F) | 0x80;

        length.push_back(lengthCount);

        while(!lengthVec.empty()){
            length.push_back(lengthVec.back());
            lengthVec.pop_back();
        }
    }else{
        length.push_back(lengthValue & 0x7F);
    }

    encodedData.insert(encodedData.end(), tag.begin(), tag.end());
    encodedData.insert(encodedData.end(), length.begin(), length.end());
    encodedData.insert(encodedData.end(), payload.begin(), payload.end());
}