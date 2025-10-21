// Ping sends out ICMP packets by opening address RAW socket, 
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

// struct addrinfo {
// 	int					ai_flags;
// 	int					ai_family;
// 	int					ai_socktype;
// 	int					ai_protocol;
// 	socklen_t			ai_addrlen;
// 	struct sockaddr		*ai_addr;
// 	char 				*ai_canonname;
// 	struct addrinfo		*ai_next;
// };

#include <icmp_request.h>

int ping_loop = 1;

// Interrupt handler
void signal_handler() { 
    ping_loop = 0; 
}

// Make a ping request
void ping_cycle(t_request *request, int sockfd) {
    printf("[FT_PING] Starting ping cycle on FD %d\n", sockfd);
	signal(SIGINT, signal_handler);

	while (ping_loop) {
		printf("%s bytes from %s (%s): icmp_seq=1 ttl=%s time=%s ms\n", 
			"64", // packet size!!
			request->reverse_hostname,
			request->target_ip,
			"62", // ttl value
			"2.00 ms"); // ping response time
		sleep(1);
	}
}

void perform_request(t_request *request) {	
	dns_resolver(request); // complete request structure

	printf("[FT_PING] -> Declaring RAW socket...\n");
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		printf("[FT_PING] SOCKET ERROR : Could not create a file descriptor. Exiting...\n");
		// free(sock_address);
		free_request(request);
		exit(EXIT_FAILURE);
	}
	else {
		printf("[FT_PING] Socket open. FD\t: %d\n", sockfd);
		ping_cycle(request, sockfd);
	}
}