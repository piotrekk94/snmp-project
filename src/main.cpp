#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ui.hpp>
#include <udp.hpp>
#include <PDU.hpp>
#include <dekoBER.hpp>
#include <koBER.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <tuple>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <signal.h>

int64_t resp;
volatile bool runSNMP = true;

void intHandler(int dummy)
{
	runSNMP = false;
}

void types_init(void){
	std::vector<seq_type> seq;

	Type INT("INTEGER", "UNIVERSAL 2", "IMPLICIT", "INTEGER", "", seq);
	Type OCT("OCTET STRING", "UNIVERSAL 4", "IMPLICIT", "OCTET STRING", "", seq);
	Type NUL("NULL", "UNIVERSAL 5", "IMPLICIT", "NULL", "", seq);
	Type OID("OBJECT IDENTIFIER", "UNIVERSAL 6", "IMPLICIT", "OBJECT IDENTIFIER", "", seq);

	types.push_back(INT);
	types.push_back(OCT);
	types.push_back(NUL);
	types.push_back(OID);
}

void handlePDU(PDU &pdu, UDP &udp, struct sockaddr_in &sa)
{
	MibObject *mibRespValue;
	MibObject mibOidR(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::OID);
	MibObject mibSeqR(PDUEnum::UNI, PDUEnum::CON, PDUEnum::SEQ);

	std::vector<uint64_t> oid;

	pdu.varBind->children[0]->children[0]->AsOid(oid);
	mibOidR.setOidData(oid);

	ObjectPath path;

	path.path.push_back(std::make_tuple("iso", -1));

	for(int i = 1; i < oid.size() - 1; i++){
		path.path.push_back(std::make_tuple("---", oid[i]));
	}

	auto object = tree->getObject(path);
	if(object){
		auto typeName = object->getTypeName();

		printf("%s\n", typeName.c_str());

		if(!typeName.compare("TimeTicks")){
			int cls = -1;
			int tag = -1;
			std::vector<unsigned int> con;

			for(auto &val : types){
				if(!val.getName().compare(typeName)){
					cls = val.getVisibility();
					tag = val.getTypeId();
					con = val.getConstraints();
				}
			}

			if(cls == -1 || tag == -1){
				cls = PDUEnum::UNI;
				tag = PDUEnum::INT;
			}

			mibRespValue = new MibObject(cls, PDUEnum::PRM, tag);

			mibRespValue->setIntData(resp);

			if(pdu.pktType == GetRequest){
				resp++;
			} else if(pdu.pktType == SetRequest){

				auto val = pdu.varBind->children[0]->children[1]->AsInt();

				if(con.size() == 2){
					if(con[0] <= val && con[1] >= val){
						resp = val;
					} else {
						printf("Constraint violation: %u <= %lld <= %u\n", con[0], val, con[1]);
					}
				} else {
					resp = val;
				}
			}
		}else{
			mibRespValue = new MibObject(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::NUL);
		}
	}else{
		mibRespValue = new MibObject(PDUEnum::UNI, PDUEnum::PRM, PDUEnum::NUL);
	}

	mibSeqR.AddChild(&mibOidR);
	mibSeqR.AddChild(mibRespValue);

	std::vector<MibObject*> varBindListResp {&mibSeqR};

	PDU enc(GetResponse, pdu.requestID, 0LLU, 0LLU, varBindListResp);

	udp.Write(&enc.encodedPkt[0], enc.encodedPkt.size(), &sa);
}

int main(int argc, char* argv[]){

	int fd;
	struct stat st;
	Object iso("iso");
	tree = new ObjectTree(&iso);

	types_init();

	p.load("RFC1213-MIB");

	std::cout<<*tree<<std::endl;

	UDP udp;

	stat("snmp.dat", &st);

	fd = open("snmp.dat", O_CREAT | O_RDWR);

	if(st.st_size == sizeof(resp)){
		read(fd, &resp, sizeof(resp));	
	}else{
		printf("No snmp sysUpTime value saved\n");
	}

	signal(SIGINT, intHandler);

	while(runSNMP){
		struct sockaddr_in sa;
		uint8_t buf[1024];
		int ret = udp.Read(buf, sizeof(buf), &sa);
		
		if(ret == -1)
			break;

		std::vector<uint8_t> vec;

		vec.assign(buf, buf + ret);

		PDU pdu(vec);

		handlePDU(pdu, udp, sa);
	}

	lseek(fd, 0, SEEK_SET);
	write(fd, &resp, sizeof(resp));

	close(fd);

	return 0;
}
