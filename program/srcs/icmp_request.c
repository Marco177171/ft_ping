// Ping sends out ICMP packets by opening a RAW socket, 
// which is separate from TCP and UDP. Since IP does not 
// have any inbuilt mechanism for sending error and control 
// messages. It depends on Internet Control Message Protocol 
// (ICMP) to provide error control. It is used for reporting 
// errors and management queries. (geeksforgeeks.org)

// TABLE OF CONVERSIONS
// htons() ->	host to network short
// htonl() ->	host to network long
// ntohs() ->	network to host short
// ntohl() ->	network to host long

#include <icmp_request.h>

void perform_request(t_request *request) {
	(void)request;
	// int sockfd;
	// char *ip_addr, *reverse_hostname;
	// struct sockaddr_in addr_con;
	// int addrlen = sizeof(addr_con);
	// char net_buf[1024]; // 1024 = default for NI_MAX_HOST
}