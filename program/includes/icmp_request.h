#ifndef ICMP_REQUEST_H
#define ICMP_REQUEST_H

#include <general.h>
#include <command.h>
#include <dns_resolver.h>

// ICMP header structure:

// -> Type (8-bit): Identifies the kind of message.
// For example, type \(8\) is an Echo Request
// (like a ping), and type \(0\) is an Echo Reply.

// -> Code (8-bit): Provides more specific information
// about the type. For example, for a "Destination 
// Unreachable" message (type \(3\)), a code of 
// \(0\) means "network unreachable," while a code
// of \(3\) means "port unreachable".

// -> Checksum (16-bit): Used to check for errors in
// the ICMP header and data.
// Extended Header (32-bit): Contains information
// about issues in the IP header, such as the
// specific byte causing an error.

// -> Data/Payload: Contains the actual data, which
// can include the first 64 bits of the original
// IP packet that generated the ICMP message. (Google)

typedef struct s_ping_stats {
	int packets_sent;
	int packets_received;
	int sequence;
	double duration;
	double total;
	double avg;
	double min;
	double max;
} t_ping_stats;

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