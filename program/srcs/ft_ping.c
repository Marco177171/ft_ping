// FLAGS:
// -f -l -n -w -W -p -r -s -T --ttl --ip-timestamp

// FORMAT:
// PING 1.1.1.1 (1.1.1.1) 56(84) bytes of data.
// PING [TARGET_IP] ([TARGET_IP]) 56(84) bytes of data.
// 64 bytes from 1.1.1.1: icmp_seq=1 ttl=51 time=52.3 ms
// 64 bytes from 1.1.1.1: icmp_seq=1 ttl=51 time=52.3 ms

// POSSIBLE ERRORS:
// TTL Expired in Transit
// Destination Host Unreachable
// Request Timed Out i.e. no reply
// Unknown Host

// The Internet Ping program works much like a sonar echo-location, 
// sending a small packet of information containing an ICMP ECHO_REQUEST 
// to a specified computer, which then sends an ECHO_REPLY packet in return. 

#include <ft_ping.h>

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

int main(int argc, char **argv) {
	if (argc == 1) {
		perror("[FT_PING] ERROR: usage : ./ft_ping [options] [target_ip]. Exiting...\n");
		exit(1);
	}

	t_request *request = malloc(sizeof(t_request));
	if (!request) {
		perror("[FT_PING] ERROR: could not allocate request structure");
		exit(1);
	}

	init_request(request);
	parse_command(request, argv);

	free_request(request);
	return 0;
}