#include <command.h>

void parse_flags(t_request *request, const char *flag_string) {
	printf("[FT_PING] Parsing flags...\n");
	if (!strcmp(flag_string, "--ttl")) {
		request->flags->ttl = 1;
		printf("[FT_PING] TTL option set.\n");
		return ;
	}
	if (!strcmp(flag_string, "--ip-timestamp")) {
		request->flags->ip_timestamp = 1;
		printf("[FT_PING] IP TIMESTAMP option set.\n");
		return ;
	}
	if (!request->flag_string) {
		request->flag_string = strdup(flag_string);
		return ;
	}
	request->flag_string = strcat(request->flag_string, flag_string);
	printf("[FT_PING] Saved flag_string: %s\n", request->flag_string);
}

void switch_flags_on(t_request *request) {
	int i = 1;

	if (request->flag_string) {
		while (request->flag_string[i]) {
			if (request->flag_string[i] == 'f')
				request->flags->flood = 1;
			else if (request->flag_string[i] == 'l')
				request->flags->preload = 1;
			else if (request->flag_string[i] == 'n')
				request->flags->numeric = 1;
			else if (request->flag_string[i] == 'w')
				request->flags->deadline = 1;
			else if (request->flag_string[i] == 'W')
				request->flags->timeout = 1;
			else if (request->flag_string[i] == 'p')
				request->flags->pattern = 1;
			else if (request->flag_string[i] == 'r')
				request->flags->bypass_routing = 1;
			else if (request->flag_string[i] == 's')
				request->flags->packetsize = 1;
			else if (request->flag_string[i] == 'T')
				request->flags->timestamp = 1;
			i++;
		}
	}
}

void free_request(t_request *request) {
	if (request->domain_name)
		free(request->domain_name);
	if (request->reverse_hostname)
		free(request->reverse_hostname);
	if (request->flag_string)
		free(request->flag_string);
	if (request->flags)
		free(request->flags);
	if (request->target_ip)
		free(request->target_ip);
	free(request);
}

void init_request(t_request *request) {
	request->domain_name = NULL;
	request->flag_string = NULL;
	request->target_ip = NULL;
	request->reverse_hostname = NULL;
	request->flags = malloc(sizeof(t_ping_flags));

	request->flags->ttl = 46;
	request->flags->ip_timestamp = 0;
	request->flags->flood = 0;
	request->flags->preload = 0;
	request->flags->numeric = 0;
	request->flags->deadline = 0;
	request->flags->timeout = 0;
	request->flags->pattern = 0;
	request->flags->packetsize = 64;
	request->flags->bypass_routing = 0;
	request->flags->timestamp = 0;
}

void parse_target(t_request *request, char *address_string) {
	// se la stringa rappresenta un IP...
	if (is_ip(address_string)) {
		request->target_ip = strdup(address_string);
		printf("[FT_PING] IP address: %s\n", request->target_ip);
	}

	// se la string rappresenta il nome di un Dominio...
	else if (is_domain_name(address_string)) {
		request->domain_name = strdup(address_string);
		printf("[FT_PING] Domain name set in structure: %s\n", request->domain_name);
	}

	// torna errore se non valida
	else {
		printf("[FT_PING] ping: %s: Name or service not known\n", address_string);
		free_request(request);
		exit(EXIT_FAILURE);
	}
}

void parse_command(t_request *request, char **argv) {
	int i = 1;
	while (argv[i]) {
		if (argv[i][0] == '-')
			parse_flags(request, argv[i]);
		else if (!argv[i + 1]) {
			// esegui un dns lookup sull'argomento. Aggiorna la struttura
			parse_target(request, argv[i]);
			// request->target_ip = strdup(argv[i]);
			// printf("Target: %s\n", request->target_ip);
		}
		i++;
	}
	if (request->flags)
		switch_flags_on(request);
}