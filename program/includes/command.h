#ifndef COMMAND_H
#define COMMAND_H

#include <general.h>
#include <checks.h>

typedef struct s_ping_flags {
	int ttl;			// -t --ttl / default = 46 -> amount of router jumps the packet can do before error
	int ip_timestamp;	// -T --ip-timestamp -> can be one of <tsonly|tsandaddr|tsprespec>
	int flood;			// -f -> send continuous packets
	int preload;		// -l -> send <preload> number of packages while waiting replies
	int numeric;
	int deadline;
	// int timeout;		// -W <time to wait for response>
	struct timeval timeout;
	int pattern;		// -p -> contents of padding byte
	int bypass_routing;
	int packetsize;		// default = 64
	int timestamp;		// -T default
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
void parse_flags(t_request *request, char **args, int i);
void switch_flags_on(t_request *request);
void free_request(t_request *request);
void parse_target(t_request *request, char *address_string);

#endif