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

char *dns_lookup(t_request *request, struct sockaddr_in *sock_address) {
	// populate request address field:
	printf("[FT_PING] DNS resolver called...\n");

	struct hostent *host_entity;
	char *ip = malloc(sizeof(char) * 1024);

	host_entity = gethostbyname(request->domain_name);
	if (!host_entity) {
		free(ip);
		return NULL;
	}
	
	printf("[FT_PING] -> Host entity found.\n");
	printf("[FT_PING] Address type\t\t: %d\n", host_entity->h_addrtype);
	printf("[FT_PING] Address list[1]\t: %s\n", host_entity->h_addr_list[1]);
	printf("[FT_PING] Address list ld\t: %ld\n", *(long *)host_entity->h_addr_list);
	
	strcpy(ip, inet_ntoa(*(struct in_addr *)host_entity->h_addr_list));
	(*sock_address).sin_family = host_entity->h_addrtype;
	(*sock_address).sin_port = htons(0); // PORT_NO (default = 0). Maybe create a global var
	(*sock_address).sin_addr.s_addr = *(long *)host_entity->h_addr_list;

	return ip;
}

void perform_request(t_request *request) {
	(void)request;
	// int sockfd;
	// char *ip_addr, *reverse_hostname; -> use request's fields
	struct sockaddr_in *sock_address = malloc(sizeof(struct sockaddr_in *));
	// int addrlen = sizeof(sock_address);
	// char net_buf[1024]; // 1024 = default for NI_MAX_HOST

	if (!request->target_ip) {
		request->target_ip = dns_lookup(request, sock_address);
		if (!request->target_ip) {
			printf("[FT_PING] dns_lookup() returned nothing. Exiting...\n");
			free_request(request);
			exit(1);
		}
	}

	printf("[FT_PING] -> Socket struct populated.\n");
	printf("[FT_PING] Socket\t\t: %d\n", sock_address->sin_addr.s_addr);
	printf("[FT_PING] Socket family\t\t: %d\n", sock_address->sin_family);
	printf("[FT_PING] Socket port\t\t: %d\n", sock_address->sin_port);

	free(sock_address);
}