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

void print_statistics(t_request *request, double min, double avg, double max) {
	if (request->domain_name)
		printf("\n--- %s ping statistics ---\n", request->domain_name);
	else
		printf("\n--- %s ping statistics ---\n", request->target_ip);
	
	printf("3 packets transmitted, 3 received, 0%% packet loss, time 2003ms\n");
	printf("rtt min/avg/max/mdev = %f/%f/%f/1.413 ms\n", min, avg, max);
}

// Make a ping request
void ping_cycle(t_request *request, int sockfd) {
    printf("[FT_PING] Starting ping cycle on FD %d\n", sockfd);
	
	if (request->domain_name) {
		printf("PING %s (%s) 56(84) bytes of data.\n", 
			request->reverse_hostname, 
			request->target_ip);
	}
	else {
		printf("PING %s (%s) 56(84) bytes of data.", 
			request->target_ip, 
			request->target_ip);
	}
		
	signal(SIGINT, signal_handler);

	t_ping_pkt *packet = malloc(sizeof(t_ping_pkt));
	if (!packet) {
		printf("[FT_PING] ERROR : could not allocate ping packet.\n");
		free_request(request);
		exit(EXIT_FAILURE);
	}
	
	clock_t start, end;
	double duration = 0, total = 0, avg = 0, min = 0, max = 0;
	int sequence = 1;
	
	while (ping_loop) {
		start = clock(); // set time = current_time
		// send ping
		// receive response
		end = clock(); // count the time it took to receive the response
		
		// catch errors
		
		duration = (double)(end - start);
		total += duration;
		if (duration < min || sequence == 1)
			min = duration;
		if (duration > max || sequence == 1)
			max = duration;

		// print current cycle's stats
		printf("%s bytes from %s (%s): icmp_seq=%d ttl=%s time=%f ms\n", 
			"64", // packet size!!
			request->reverse_hostname,
			request->target_ip,
			sequence,
			"46", // ttl value
			duration); // ping response time
		sleep(1);
		sequence++;
	}
	avg = total / sequence;
	print_statistics(request, min, avg, max);
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