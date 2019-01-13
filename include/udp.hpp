#pragma once
#include <cstdint>
#include <cstddef>

#include <sys/socket.h>

class UDP {
private:
	int s;
public:
	UDP(int port = 161);
	~UDP(void);
	int Read(uint8_t *buf, int len, struct sockaddr_in *sa);
	int Write(uint8_t *buf, int len, struct sockaddr_in *sa);
};
