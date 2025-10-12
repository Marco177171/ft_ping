#ifndef COMMAND_H
#define COMMAND_H

#include <general.h>
#include <checks.h>

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
	char *domain_name;		// provided argument
	char *target_ip;		// IP address
	char *reverse_hostname;	// extrapolated from the dns lookup
	char *flag_string;		// flags in the command
	t_ping_flags *flags;	// flag structure
} t_request;

// command parsing and structure management
void init_request(t_request *request);
void parse_command(t_request *request, char **argv);
void parse_flags(t_request *request, const char *flag_string);
void switch_flags_on(t_request *request);
void free_request(t_request *request);
void parse_target(t_request *request, char *address_string);

#endif