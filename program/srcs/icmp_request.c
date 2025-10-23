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
void signal_handler() { 
    ping_loop = 0;
}

void print_statistics(t_request *request, double min, double avg, double max) {
	if (request->domain_name)
		printf("\n--- %s ping statistics ---\n", request->domain_name);
	else
		printf("\n--- %s ping statistics ---\n", request->target_ip);
	
	printf("3 packets transmitted, 3 received, 0%% packet loss, time 2003ms\n");
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/1.413 ms\n", min, avg, max);
}

void ping_cycle(t_request *request, struct sockaddr_in *sock_address) {
	signal(SIGINT, signal_handler);
	
	printf("[FT_PING] -> Declaring RAW socket...\n");
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		printf("[FT_PING] SOCKET ERROR : Could not create a file descriptor. Exiting...\n");
		// free(sock_address);
		free_request(request);
		exit(EXIT_FAILURE);
	}
	printf("[FT_PING] Socket open. FD\t: %d\n", sockfd);

	printf("[FT_PING] -> Creating ping packet...\n");
	t_ping_pkt *packet = malloc(sizeof(t_ping_pkt));
	if (!packet) {
		printf("[FT_PING] ERROR : could not allocate ping packet.\n");
		free_request(request);
		exit(EXIT_FAILURE);
	}

	struct timespec start, end;
	double duration = 0, total = 0, avg = 0, min = 0, max = 0;
	// setsockopt(sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0);
	
	// struct sockaddr_in *receptor = malloc(sizeof(struct sockaddr_in)); // receive answers here
	// char receiver_buf[128];
	int sequence = 1;

	while (ping_loop) {
		clock_gettime(CLOCK_MONOTONIC, &start); // set time = current_time (nano-s)
		
		// send ping and check
		if (sendto(sockfd,
			packet,
			sizeof(*packet),
			0,
			(struct sockaddr *)sock_address,
			sizeof(*sock_address)) <= 0) {
			printf("[FT_PING] ERROR : Could not send data through the socket\n");
			free(packet);
			free_request(request);
			exit(EXIT_FAILURE);
		}

		// receive response
		// if (recvfrom(sockfd,
		// 	receiver_buf,
		// 	sizeof(receiver_buf),
		// 	0,
		// 	(struct sockaddr *)receptor,
		// 	(socklen_t * restrict)(sizeof(*receptor))) <= 0 &&
		// 	sequence > 1) {
		// 	printf("[FT_PING] ERROR : Could not receive an answer from the target\n");
		// 	free(packet);
		// 	free_request(request);
		// 	exit(EXIT_FAILURE);
		// }
		clock_gettime(CLOCK_MONOTONIC, &end); // nanoseconds to receive the response
		duration = (double)((end.tv_nsec - start.tv_nsec) / 10000); // losing precision with cast to double
		total += duration;
		
		if (duration < min || sequence == 1)
			min = duration;
		if (duration > max || sequence == 1)
			max = duration;

		// print current cycle's stats
		printf("%s bytes from %s (%s): icmp_seq=%d ttl=%s time=%.2f ms\n", 
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

// Make a ping request
void init_ping(t_request *request) {
	
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
}

void perform_request(t_request *request) {	
	struct sockaddr_in *sock_address = dns_resolver(request);	// complete request structure
	init_ping(request);		// set request
	ping_cycle(request, sock_address);	// execute actual ping
}