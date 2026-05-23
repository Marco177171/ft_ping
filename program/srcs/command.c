#include <command.h>

void set_preload(t_request *request, char **argv, int i) {
	int j = 0;
	while (argv[i + 1][j]) {
		if (!isdigit(argv[i + 1][j])) {
			printf("[ERROR] : flag \"%s\" has not a valid argument argument\n", argv[i]);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		j++;
	}
	int preload = atoi(argv[i+ 1]);
	if (preload > 3) {
		printf("[ERROR] : ping: cannot set preload to value greater than 3: %s\n", argv[i + 1]);
		free_request(request);
		exit(EXIT_FAILURE);
	}
	request->flags->preload = preload;
}

void set_interval(t_request *request, char *interval_string) {
	int i = 0;
	while (interval_string[i]) {
		if (!isdigit(interval_string[i])) {
			printf("ping: option argument contains garbage: %s\n", interval_string);
			printf("ping: this will become fatal error in the future\n");
			printf("ping: usage error: Destination address required\n");
			free_request(request);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	int interval = atoi(interval_string);
	request->flags->interval = interval;
}

void set_deadline(t_request *request, char *dealine_string) {
	int i = 0;
	while (dealine_string[i]) {
		if (!isdigit(dealine_string[i])) {
			printf("ping: option argument contains garbage: %s\n", dealine_string);
			printf("ping: this will become fatal error in the future\n");
			printf("ping: usage error: Destination address required\n");
			free_request(request);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	int deadline = atoi(dealine_string);
	request->flags->deadline = deadline;
}

void set_timeout(t_request *request, char *timeout_seconds) {
	int i = 0;
	while (timeout_seconds[i]) {
		if (!isdigit(timeout_seconds[i])) {
			printf("ping: invalid number '%s'\n", timeout_seconds);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	int timeout = atoi(timeout_seconds);
	request->flags->deadline = timeout;
}

void set_packet_size(t_request *request, char *size_string) {
	int i = 0;
	while (size_string[i]) {
		if (!isdigit(size_string[i])) {
			printf("ping: invalid number '%s'\n", size_string);
			free_request(request);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	int packet_size = atoi(size_string) + 8; // at least 1 byte
	request->flags->packetsize = packet_size;
}

// -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
void set_flag(t_request *request, char **argv, int i) {
	if (argv[i][1] == 'i')
		set_interval(request, argv[i + 1]);
	if (argv[i][1] == 'f')
		request->flags->flood = 1;
	else if (argv[i][1] == 'l')
		set_preload(request, argv, i);
	else if (argv[i][1] == 'n')
		request->flags->numeric = 1;
	else if (argv[i][1] == 'w')
		set_deadline(request, argv[i + 1]);
	else if (argv[i][1] == 'W')
		set_timeout(request, argv[i + 1]);
	else if (argv[i][1] == 's')
		set_packet_size(request, argv[i + 1]);
	// NOT MANAGED:
	// else if (argv[i][1] == 'p')
	// 	// pad bytes (???)
	// else if (argv[i][1] == 'r')
	// 	// Bypass the normal routing tables
	// else if (argv[i][1] == 'T' || !strcmp(argv[i], "--ip-timestamp"))
	// 	// set timestamp
	// else if (argv[i][1] == 't' || !strcmp(argv[i], "--ttl"))
	// 	// time to live
}

void parse_flags(t_request *request, char **argv, int i) {
	// la stringa in i è una flag.
	// controlla se è tra le flag richieste
	// se non lo è, torna un errore
	if (strcmp(argv[i], "-i") != 0
		&& strcmp(argv[i], "-f") != 0
		&& strcmp(argv[i], "-l") != 0
		&& strcmp(argv[i], "-n") != 0
		&& strcmp(argv[i], "-w") != 0
		&& strcmp(argv[i], "-W") != 0
		&& strcmp(argv[i], "-s") != 0
		// && strcmp(argv[i], "-p") != 0
		// && strcmp(argv[i], "-r") != 0
		// && strcmp(argv[i], "-T") != 0
		// && strcmp(argv[i], "-ttl") != 0
		// && strcmp(argv[i], "-t") != 0
		// && strcmp(argv[i], "--ip-timestmp") != 0
		) {
		printf("ping: unrecognized option: %s\n", argv[i]);
		free_request(request);
		exit (EXIT_FAILURE);
	}
	// Se tutto è giusto, registra la flag
	set_flag(request, argv, i);
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
	// Allocate and set flags
	request->flags = malloc(sizeof(t_ping_flags));
	request->flags->interval = 1;
	request->flags->ttl = 46;
	request->flags->ip_timestamp = 0;
	request->flags->flood = 0;
	request->flags->preload = 0;
	request->flags->numeric = 0;
	request->flags->deadline = 0;
	request->flags->timeout.tv_sec = 0;		// seconds
	request->flags->timeout.tv_usec = 0;	// milliseconds
	request->flags->pattern = 0;
	request->flags->packetsize = 64;
	request->flags->bypass_routing = 0;
	request->flags->timestamp = 0;
}

void parse_target(t_request *request, char *address_string) {
	// se la stringa rappresenta un IP...
	if (is_ip(address_string)) {
		request->target_ip = strdup(address_string);
		// printf("[FT_PING] IP address: %s\n", request->target_ip);
	}

	// se la string rappresenta il nome di un Dominio...
	else if (is_domain_name(address_string)) {
		request->domain_name = strdup(address_string);
		// printf("[FT_PING] Domain name set in structure: %s\n", request->domain_name);
	}

	else if (!strncmp(address_string, "localhost", 9)) {
		request->target_ip = strdup("127.0.0.1");
		// printf("[FT_PING] Localhost address set: %s\n", request->target_ip);
	}

	// torna errore se non valida
	else {
		// printf("[FT_PING] ping: %s: Name or service not known\n", address_string);
		free_request(request);
		exit(EXIT_FAILURE);
	}
}

void parse_command(t_request *request, char **argv) {
	int i = 1;
	while (argv[i]) {
		if (argv[i][0] == '-')
			parse_flags(request, argv, i);
		else if (!argv[i + 1])
			parse_target(request, argv[i]);
		i++;
	}
}