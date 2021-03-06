
#include "pch.h"
#include <iostream>
#include <string>
#include <map>

char AI_FAMILY[][20] = {
	"AF_UNSPEC","AF_UNIX","AF_INET","AF_IMPLINK","AF_PUP",
	"AF_CHAOS","AF_NS/AF_IPX","AF_ISO/AF_OSI","AF_ECMA","AF_DATAKIT",
	"AF_CCITT","AF_SNA","AF_DECnet","AF_DLI","AF_LAT",
	"AF_HYLINK","AF_APPLETALK","AF_NETBIOS","AF_VOICEVIEW","AF_FIREFOX",
	"AF_UNKNOWN1","AF_BAN","AF_ATM","AF_INET6","AF_CLUSTER",
	"AF_12844","AF_IRDA","!unknown!","AF_NETDES"
};

char AI_SOCKTYPE[][20] = { "!unknown!","SOCK_STREAM", "SOCK_DGRAM", "SOCK_RAW", "SOCK_RDM", "SOCK_SEQPACKET" };

std::map<int, const std::string> AI_PROTOCOL = {
	{0, "IPPROTO_IP"},
	{1, "IPPROTO_ICMP"},
	{2, "IPPROTO_IGMP"},
	{3, "IPPROTO_GGP"},
	{6, "IPPROTO_TCP"},
	{7, "IPPORT_ECHO"},
	{9, "IPPORT_DISCARD"},
	{11, "IPPORT_SYSTAT"},
	{12, "IPPROTO_PUP"},
	{13, "IPPORT_DAYTIME"},
	{15, "IPPORT_NETSTAT"},
	{17, "IPPROTO_UDP"},
	{21, "IPPORT_FTP"},
	{22, "IPPROTO_IDP"},
	{23, "IPPORT_TELNET"},
	{25, "IPPORT_SMTP"},
	{37, "IPPORT_TIMESERVER"},
	{42, "IPPORT_NAMESERVER"},
	{43, "IPPORT_WHOIS"},
	{57, "IPPORT_MTP"},
	{69, "IPPORT_TFTP"},
	{77, "IPPROTO_ND|IPPORT_RJE"},
	{79, "IPPORT_FINGER"},
	{87, "IPPORT_TTYLINK"},
	{95, "IPPORT_SUPDUP"},
	{255, "IPPROTO_RAW"},
	{256, "IPPROTO_MAX"},
	{512, "IPPORT_EXECSERVER|IPPORT_BIFFUDP"},
	{513, "IPPORT_LOGINSERVER|IPPORT_WHOSERVER"},
	{514, "IPPORT_CMDSERVER"},
	{520, "IPPORT_EFSSERVER|IPPORT_ROUTESERVER"},
	{1024, "IPPORT_RESERVED"}
};

std::map<int, const std::string> AI_FLAGS = {
	{0x00000001, "AI_PASSIVE"},
	{0x00000002, "AI_CANONNAME"},
	{0x00000004, "AI_NUMERICHOST"},
	{0x00000008, "AI_NUMERICSERV"},
	{0x00000010, "AI_DNS_ONLY"},
	{0x00000100, "AI_ALL"},
	{0x00000400, "AI_ADDRCONFIG"},
	{0x00000800, "AI_V4MAPPED"},
	{0x00004000, "AI_NON_AUTHORITATIVE"},
	{0x00008000, "AI_SECURE"},
	{0x00010000, "AI_RETURN_PREFERRED_NAMES"},
	{0x00020000, "AI_FQDN"},
	{0x00040000, "AI_FILESERVER"},
	{0x00080000, "AI_DISABLE_IDN_ENCODING"},
	{0x80000000, "AI_EXTENDED"},
	{0x40000000, "AI_RESOLUTION_HANDLE"}
};
typedef std::map<int, const std::string>::const_iterator T_AI_FLAGS_ITER;

void print_addrinfo(addrinfo *p) {
	printf("===========================\nfamily:    %3d  %s\n", p->ai_family, AI_FAMILY[p->ai_family]);
	printf("socktype:  %3d  %-20sprotocol:  %3d  %-20s\n", p->ai_socktype, AI_SOCKTYPE[p->ai_socktype], p->ai_protocol, AI_PROTOCOL[p->ai_protocol].c_str());
	printf("flags: %08X  ", p->ai_flags);
	bool first_flag = true;
	for (T_AI_FLAGS_ITER it = AI_FLAGS.cbegin(); it != AI_FLAGS.cend(); it++) {
		if (p->ai_flags & it->first) {
			printf(first_flag ? "%s" : " | %s", it->second.c_str());
			first_flag = false;
		}
	}
	printf("\naddrlen:    %d\naddr:   ", p->ai_addrlen);
	for (uint32_t i = 0; i < p->ai_addrlen; i++) {
		printf(" %02X", (int)((uint8_t*)(p->ai_addr))[i]);
	}

	void *addr_addr = p->ai_family == AF_INET ?
		(void*)&((sockaddr_in*)p->ai_addr)->sin_addr :
		p->ai_family == AF_INET6 ?
		(void*)&((sockaddr_in6*)p->ai_addr)->sin6_addr :
		nullptr;
	if (addr_addr) {
		char temp[256];
		inet_ntop(p->ai_family, addr_addr, temp, sizeof(temp));
		temp[sizeof(temp) - 1] = 0;
		printf("\naddr-ntop: %s\n\n", temp);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("\n  Usage: %s HOSTNAME PORTNAME\n\n", argv[0]);
		return 1;
	}

	printf("Showing result of getaddrinfo(\"%s\", \"%s\")\n\n", argv[1], argv[2]);

	WSAData wsadata;
	printf("WSAStartup: %d\n", WSAStartup(MAKEWORD(2, 2), &wsadata));

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo *result = nullptr;

	int ret = getaddrinfo(argv[1], argv[2], &hints, &result);
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	printf("getaddrinfo returns: %d  %X\nWSAGetLastError: %d  ", ret, ret, WSAGetLastError());
	_putws(s);
	puts("");
	LocalFree(s);

	addrinfo *p = result;
	while (p) {
		print_addrinfo(p);
		p = p->ai_next;
	}

	return 0;
}

