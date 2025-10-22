#ifndef ICMP_REQUEST_H
#define ICMP_REQUEST_H

#include <general.h>
#include <command.h>
#include <dns_resolver.h>

typedef struct s_ping_pkt {
	struct icmphdr hdr;
	char msg[64 - sizeof(struct icmphdr)];
} t_ping_pkt;

typedef struct s_ping_settings {
	int packet_size;		// 64 by default
	int port;				// 64 by default
	int ping_sleep_rate;	// 1000000 microseconds bby default
	int receive_timeout;	// 1 by default
	t_ping_pkt *packet;
} t_ping_settings;

void perform_request(t_request *request);

#endif