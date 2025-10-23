#ifndef DNS_RESOLVER_H
#define DNS_RESOLVER_H

#include <general.h>
#include <command.h>

char *dns_lookup(char *domain_name, struct sockaddr_in *sock_address);
char *reverse_dns_lookup(char *ip, struct sockaddr_in *sock_addr);
struct sockaddr_in *dns_resolver(t_request *request);

#endif