#include <checks.h>

int is_ip(char *string) {
	int i = 0, dots = 0;
	while (string[i]) {
		if ((!isdigit(string[i]) && string[i] != '.') || dots > 3) {
			printf("[FT_PING] Not an IP (invalid characters). Returning...\n");
			return 0;
		}
		if (string[i] == '.')
			dots++;
		i++;
	}
	printf("[FT_PING] %s is an IP\n", string);
	return 1;
}

int is_domain_name(char *address_string) {
	int i = 0, dots = 0;
	while (address_string[i]) {
		if (!isalnum(address_string[i]) &&
			address_string[i] != '.')
				return 0;
		if (address_string[i] == '.')
			dots++;
		i++;
	}
	if (dots == 0) {
		printf("[FT_PING] No dots in provided string. It's not a valid domain name\n");
		return 0;
	}
	printf("[FT_PING] %s is a valid domain name\n", address_string);
	return 1;
}
