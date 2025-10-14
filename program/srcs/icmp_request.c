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

char *reverse_dns_lookup(char *ip) {
    printf("[FT_PING] Reverse DNS lookup called...\n");

	struct sockaddr_in temp;

	temp.sin_family = AF_INET;
	temp.sin_addr.s_addr = inet_addr(ip);
	socklen_t len = sizeof(struct sockaddr_in);

	char buffer[1025];
	
	if (getnameinfo((struct sockaddr *)&temp, len, buffer, sizeof(buffer), NULL, 0, NI_NAMEREQD)) {
        printf("[FT_PING] ERROR : Could not resolve reverse lookup for \"%s\"\n", ip);
        return NULL;
    }

	char *result = malloc(sizeof(char) * (strlen(buffer) + 1));
	if (!result) {
		printf("[FT_PING] ERROR : Could not allocate result string. Returning (null)...\n");
		return NULL;
	}

	strcpy(result, buffer);
	return result;
}

void perform_request(t_request *request) {
    struct sockaddr_in *sock_address = malloc(sizeof(struct sockaddr_in));
    if (!sock_address) {
        printf("[FT_PING] ERROR : Could not allocate sockaddr_in* struct for socket address. Exiting...");
        free_request(request);
        exit(EXIT_FAILURE);
    }

	// sock_address->sin_addr.s_addr = NULL;
	// sock_address->sin_family = NULL;
	// sock_address->sin_port = 0;
	// sock_address->sin_zero = 0;

	// int addrlen = sizeof(sock_address);
	// char net_buf[1024]; // 1024 = default for NI_MAX_HOST

	if (!request->target_ip) {
		request->target_ip = dns_lookup(request, sock_address);
		if (!request->target_ip) {
			printf("[FT_PING] ERROR : dns_lookup() returned nothing. Exiting...\n");
			free(sock_address);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		printf("[FT_PING] target ip of the target : %s\n", request->target_ip);
	}

	printf("[FT_PING] -> Socket struct populated.\n");
	printf("[FT_PING] Socket address\t: %d\n", sock_address->sin_addr.s_addr);
	printf("[FT_PING] Socket family\t\t: %d\n", sock_address->sin_family);
	printf("[FT_PING] Socket port\t\t: %d\n", sock_address->sin_port);

    if (!request->reverse_hostname) {
        request->reverse_hostname = reverse_dns_lookup(request->target_ip);
        if (!request->reverse_hostname) {
            printf("[FT_PING] ERROR : reverse_dns_lookup() returned nothing. Exiting...\n");
			free(sock_address);
            free_request(request);
            exit(EXIT_FAILURE);
        }
    }

	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	printf("[FT_PING] SOCKET: %d\n", sockfd);
	if (sockfd < 0) {
		printf("[FT_PING] SOCKET ERROR : Could not create a file descriptor. Exiting...\n");
		free(sock_address);
		free_request(request);
		exit(EXIT_FAILURE);
	}
	else {
		printf("[FT_PING] Newly created socket received file descrptor # %d\n", sockfd);
	}

    // The socket can now be used to send the request

	free(sock_address);
}
