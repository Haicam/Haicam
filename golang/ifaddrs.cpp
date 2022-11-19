#include "ifaddrs.h"

#ifdef __ANDROID__
#include "ifaddrs-android.h"

#ifdef __cplusplus
extern "C" {
#endif
const char* getInterfacesAndIPs()
{
    int NI_MAXHOST = 64;
    int LINE_LEN = 128;
    int MAX_IPS = 10;

	struct ifaddrs *ifaddr;
	int family, s;
	char ip[NI_MAXHOST];
    char mask[NI_MAXHOST];
    char line[LINE_LEN];
    char out[LINE_LEN * MAX_IPS];
    int count = 0;

	if (getifaddrs(&ifaddr) == -1) {
		printf("InterfacesAndIPs: ERROR\n");
		return strdup("[]");
	}

    memset(out, 0, LINE_LEN * MAX_IPS);
    strcat(out, "[");

	for (struct ifaddrs *ifa = ifaddr; ifa != NULL;	ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, "lo") == 0)
			continue;
		family = ifa->ifa_addr->sa_family;

        memset(ip, 0, NI_MAXHOST);

		if (family == AF_INET) {
            inet_ntop(family, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip, NI_MAXHOST);
            inet_ntop(family, &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr, mask, NI_MAXHOST);
            sprintf(line, "{\"interface\":\"%s\", \"ip\":\"%s\",\"mask\":\"%s\",\"isIPv4\":%i}", ifa->ifa_name, ip , mask, 1);
		} else if (family == AF_INET6) {
            // TODO we will support ipv6 later
            //inet_ntop(family, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr, ip, NI_MAXHOST);
            //sprintf(line, "{\"interface\":\"%s\", \"ip\":\"%s\",\"isIPv4\":%i}", ifa->ifa_name, ip , 0);
        }

        if (strlen(ip) > 0) {
            count ++;
            if (strlen(out) > 1) strcat(out, ",");
            strcat(out, line);
        }

        if (count == MAX_IPS) break;
	}
    strcat(out, "]");

	freeifaddrs(ifaddr);

    printf("getInterfacesAndIPs json: %s \n", out);

    return strdup(out);
}

#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus
extern "C" {
#endif

const char* getInterfacesAndIPs() {
    return strdup("[]");
}

#ifdef __cplusplus
}
#endif

#endif