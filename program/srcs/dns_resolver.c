#include <dns_resolver.h>

char *dns_lookup(char *domain_name, struct sockaddr_in *sock_address) {

	printf("[FT_PING] -> DNS resolver called...\n");

	struct addrinfo *address = NULL;
	getaddrinfo(domain_name, NULL, NULL, &address);

	char buf_string[128];
	// struct sockaddr_in *socket_address = NULL;

	while (address->ai_next) {
		sock_address = (struct sockaddr_in *)address->ai_addr;

		inet_ntop(address->ai_family, 
			&sock_address->sin_addr, 
			buf_string, 
			sizeof(buf_string));

		printf("[FT_PING] Address flags\t\t: %d\n", address->ai_flags);
		printf("[FT_PING] Address sock type\t: %d\n", address->ai_socktype);
		printf("[FT_PING] Address family\t: %d\n", address->ai_family);

		address = address->ai_next;
	}

	char *result = strdup(buf_string);
	return result;
}

char *reverse_dns_lookup(char *ip, struct sockaddr_in *sock_addr) {
	printf("[FT_PING] -> Reverse DNS lookup called...\n");

	sock_addr->sin_family = AF_INET;
	sock_addr->sin_addr.s_addr = inet_addr(ip);
	socklen_t len = sizeof(struct sockaddr_in);

	char buffer[128];

	if (getnameinfo((const struct sockaddr *)sock_addr, len, buffer, sizeof(buffer), NULL, 0, NI_NAMEREQD)) {
		printf("[FT_PING] ERROR : Could not resolve reverse lookup for \"%s\"\n", ip);
		return NULL;
	}

	char *domain_name = strdup(buffer);
	if (!domain_name) {
		printf("[FT_PING] ERROR : Could not allocate domain_name string. Returning (null)...\n");
		return NULL;
	}

	printf("[FT_PING] Domain name found\t: %s\n", domain_name);

	return domain_name;
}

struct sockaddr_in * dns_resolver(t_request *request) {
	struct sockaddr_in *sock_address = malloc(sizeof(struct sockaddr_in));
	if (!sock_address) {
		printf("[FT_PING] ERROR : Could not allocate sockaddr_in* struct for socket address. Exiting...");
		free_request(request);
		exit(EXIT_FAILURE);
	}

	if (!request->target_ip) {
		request->target_ip = dns_lookup(request->domain_name, sock_address);
		if (!request->target_ip) {
			printf("[FT_PING] ERROR : dns_lookup() returned nothing. Exiting...\n");
			free(sock_address);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		printf("[FT_PING] found target's ip\t: %s\n", request->target_ip);
	}

	if (!request->reverse_hostname) {
		request->reverse_hostname = reverse_dns_lookup(request->target_ip, sock_address);
		if (!request->reverse_hostname) {
			printf("[FT_PING] ERROR : reverse_dns_lookup() returned nothing. Exiting...\n");
			free(sock_address);
			free_request(request);
			exit(EXIT_FAILURE);
		}
	}

	printf("[FT_PING] -> Request structure populated.\n");
	printf("[FT_PING] Domain name\t\t: %s\n", request->domain_name);
	printf("[FT_PING] Reverse hostname\t: %s\n", request->reverse_hostname);
	printf("[FT_PING] Target IP\t\t: %s\n", request->target_ip);

	printf("[FT_PING] -> Socket structure populated.\n");
	printf("[FT_PING] Socket address\t: %d\n", sock_address->sin_addr.s_addr);
	printf("[FT_PING] Socket family\t\t: %d\n", sock_address->sin_family);
	printf("[FT_PING] Socket port\t\t: %d\n", sock_address->sin_port);

	// free(sock_address);
	return sock_address;
}