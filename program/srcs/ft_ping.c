// FLAGS:
// -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp

// FORMAT:
// PING 1.1.1.1 (1.1.1.1) 56(84) bytes of data.
// PING [TARGET_IP] ([TARGET_IP]) 56(84) bytes of data.
// 64 bytes from 1.1.1.1: icmp_seq=1 ttl=51 time=52.3 ms

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct s_ping_flags {
	int ttl;
	int ip_timestamp;
	int flood;
	int preload;
	int numeric;
	int deadline;
	int timeout;
	int pattern;
	int bypass_routing;
	int packetsize;
	int timestamp;
} t_ping_flags;

typedef struct s_request {
	char *target_ip;
	char *flag_string;
	t_ping_flags *flags;
} t_request;

void f_flag () {
	printf("Managing f flag...\n");
}

void parse_flags(t_request *request, const char *flag_string) {
	printf("Saving flags...\n");
	if (!strcmp(flag_string, "--ttl")) {
		request->flags->ttl = 1;
		printf("TTL option set.\n");
		return ;
	}
	if (!strcmp(flag_string, "--ip-timestamp")) {
		request->flags->ip_timestamp = 1;
		printf("IP TIMESTAMP option set.\n");
		return ;
	}
	if (!request->flag_string)
		request->flag_string = strdup("\0");
	char *updated_flag_string = strcat(request->flag_string, flag_string);
	request->flag_string = strdup(updated_flag_string);
	free(updated_flag_string);
	printf("Saved flag_string: %s\n", request->flag_string);
}

void flood() {
	printf("flooding option activated");
}

// -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp
void switch_on_flags(t_request *request) {
	int i = 1;
	
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

void free_request(t_request *request) {
	if (request->flag_string)
		free(request->flag_string);
	if (request->flags)
		free(request->flags);
	if (request->target_ip)
		free(request->target_ip);
	free(request);
}

void init_request(t_request *request) {
	request->flag_string = NULL;
	request->target_ip = NULL;
	request->flags = malloc(sizeof(t_ping_flags));
	request->flags->ttl = 0;
	request->flags->ip_timestamp = 0;
	request->flags->flood = 0;
	request->flags->preload = 0;
	request->flags->numeric = 0;
	request->flags->deadline= 0;
	request->flags->timeout = 0;
	request->flags->pattern = 0;
	request->flags->bypass_routing = 0;
	request->flags->timestamp = 0;
}

int main(int argc, char **argv) {
	(void)argc;
	
	t_request *request = malloc(sizeof(t_request));
	if (!request) {
		perror("ERROR: could not allocate request structure");
		exit(1);
	}
	
	init_request(request);

	
	// trova le flag, salvale nella struttura;
	int i = 1;
	while (argv[i]) {
		
		if (argv[i][0] == '-')
			parse_flags(request, argv[i]);
		else if (!argv[i + 1]) {
			request->target_ip = strdup(argv[i]);
			printf("Target: %s\n", request->target_ip);
		}

		i++;
	}

	if (request->flags)
		switch_on_flags(request);

	free_request(request);
	return 0;
}
