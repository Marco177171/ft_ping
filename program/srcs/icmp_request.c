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

void print_statistics(t_request *request, t_ping_stats *stats) {
	if (request->domain_name)
		printf("\n--- %s ping statistics ---\n", request->domain_name);
	else
		printf("\n--- %s ping statistics ---\n", request->target_ip);

	printf("%d packets transmitted, %d received, 0%% packet loss, time %fms\n", stats->packets_sent, stats->packets_received, stats->duration);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/1.413 ms\n", stats->min, stats->avg, stats->max);
}

void init_stats(t_ping_stats *stats) {
	stats->packets_sent = 0;
	stats->packets_received = 0;
	stats->sequence = 0;
	stats->duration = 0;
	stats->total = 0;
	stats->avg = 0;
	stats->min = 0;
	stats->max = 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void define_socket_options(int *sockfd, t_request *request) {
	// TTL at IP level
	if (setsockopt(*sockfd, 
		SOL_IP, 
		IP_TTL, 
		&request->flags->ttl, 
		sizeof(request->flags->ttl)) != 0) {
		printf("[FT_PING] ERROR : Setting socket options to TTL failed!\n");
		return;
	}
	// TIMEOUT at SOCKET level
	setsockopt(*sockfd, 
		SOL_SOCKET, 
		SO_RCVTIMEO, 
		&request->flags->timeout, 
		sizeof(request->flags->timeout));
}


void ping_cycle(t_request *request, struct sockaddr_in *sock_address) {
	signal(SIGINT, signal_handler);

	// printf("[FT_PING] -> Declaring RAW socket...\n");

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

	define_socket_options(&sockfd, request);

	struct sockaddr_in *receptor = malloc(sizeof(struct sockaddr_in)); // receive answers here
	socklen_t receptor_len = sizeof(*receptor);

	struct timespec start, end;
	t_ping_stats *stats = malloc(sizeof(t_ping_stats));
	init_stats(stats);
	// double duration = 0, total = 0, avg = 0, min = 0, max = 0;
	char receiver_buf[1024];
	
	struct iphdr *ip_hdr = (struct iphdr *)receiver_buf;
	int ip_header_len = ip_hdr->ihl * 4;

	printf("[FT_PING] Starting cycle...\n");

	while (ping_loop) {
		// Init packet structure
		packet->hdr.type = ICMP_ECHO; // set ping header
		packet->hdr.un.echo.id = getpid(); // assign id = this process id
		packet->msg[sizeof(packet->msg) - 1] = '\0';
		packet->hdr.checksum = 0;
		packet->hdr.checksum = checksum(packet, sizeof(*packet));

		// printf("[FT_PING] Packet filled. header_type : %d | id : %d | message : %s\n",
		// 	packet->hdr.type, packet->hdr.un.echo.id, packet->msg);

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
		stats->packets_sent++;

		// printf("[FT_PING] packet N %d sent.\n", sequence);

		// receive response and check
		if (recvfrom(sockfd,
			receiver_buf,
			sizeof(receiver_buf),
			0,
			(struct sockaddr *)receptor,
			&receptor_len) <= 0) {
			printf("[FT_PING] ERROR : Could not receive an answer from the target\n");
			free(packet);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		stats->packets_received++;

		struct icmphdr *icmp_hdr = (struct icmphdr *)(receiver_buf + ip_header_len);

		if (icmp_hdr->type == ICMP_ECHOREPLY &&
			icmp_hdr->un.echo.id == getpid()) {
			// Got a valid reply
		} else {
			// Maybe not your reply — loop again or ignore
		}

		clock_gettime(CLOCK_MONOTONIC, &end); // nanoseconds to receive the response
		stats->duration = (double)(end.tv_sec - start.tv_sec) * 1000.0
			+ (double)(end.tv_nsec - start.tv_nsec) / 1e6;
		stats->total += stats->duration;

		if (stats->duration < stats->min || stats->sequence == 1)
			stats->min = stats->duration;
		if (stats->duration > stats->max || stats->sequence == 1)
			stats->max = stats->duration;

		// print current cycle's stats
		printf("%s bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms\n", 
			"64", // packet size!!
			request->reverse_hostname,
			request->target_ip,
			stats->sequence,
			request->flags->ttl,
			stats->duration); // ping response time
		sleep(1);
		stats->sequence++;
	}
	if (stats->sequence > 1)
		stats->avg = stats->total / (stats->sequence - 1);
	else
		stats->avg = 0;
	print_statistics(request, stats);
}

// Make a ping request
void initial_message(t_request *request) {
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
	struct sockaddr_in *sock_address = dns_resolver(request); // complete request structure
	initial_message(request); // init request
	ping_cycle(request, sock_address); // execute actual ping
}