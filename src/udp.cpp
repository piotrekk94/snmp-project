#include "udp.hpp"

#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

UDP::UDP(int port)
{
	struct sockaddr_in sa = {0};
	int ret;

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(port);

	s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	ret = bind(s, (struct sockaddr *)&sa, sizeof(sa));

	if(ret != -1)
		printf("UDP: Socket created on port %d\n", port);
}

UDP::~UDP(void)
{
	int ret;

	close(s);

	if(ret != -1)
		printf("UDP: Socket closed");
}

int UDP::Read(uint8_t *buf, int len, struct sockaddr_in *sa)
{
	int ret;
	unsigned int fromlen = sizeof(struct sockaddr);

	ret = recvfrom(s, buf, len, 0, (struct sockaddr *)sa, &fromlen);

	if(ret != -1)
		printf("UDP: %d bytes received\n", ret);

	return ret;
}

int UDP::Write(uint8_t *buf, int len, struct sockaddr_in *sa)
{
	int ret;

	ret = sendto(s, buf, len, 0, (struct sockaddr *)sa, sizeof(struct sockaddr));

	if(ret != -1)
		printf("UDP: %d bytes transmitted\n", ret);

	return ret;
}
