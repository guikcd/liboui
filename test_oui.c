#include <stdlib.h>
#include <stdio.h>
#include "oui.h"

int main (void) {
	char buf[ORGANIZATION_LENGTH];
	int res = 0;

	create_hash();

	/* virtually generated, not found in database */
	const char *failed_mac = "52:54:00:8d:b8:20";

	const char *macs[MAC_ADDRESS_LENGTH];
	macs[0] = "6cf049";
	macs[1] = "6CF049";

	macs[2] = "6c:f0:49";
	macs[3] = "6C:F0:49";
	macs[4] = "6c-f0-49";
	macs[5] = "6C-F0-49";

	macs[6] = "6c:f0:49:e0-95-9f";
	macs[7] = "6C:F0:49:E0-95-9F";
	macs[8] = "6c-f0-49-e0-95-9f";
	macs[9] = "6C-F0-49-E0-95-9F";
	macs[10] = failed_mac;
	macs[11] = "0008a1877afa";
	/* vendor name out of bounds (70 instead of 50) */
	macs[12] = "60-89-B7";
	/* oui out of bound */
	macs[13] = "6C-F0-49-E0-95-9FA";
	macs[14] = "6C:F0:49:E0:95:9FA";
	/* missing char */
	macs[15] = "6CF04";
	/* invalid */
	macs[16] = "6C-F0;49";
	int i = 0;

	for (i; i < sizeof(macs) / sizeof(*macs); i++) {
		//printf("%s\n", macs[i]);
		res = get_organization(buf, macs[i]);
		if (res == 0) {
			printf("OK: %s (%s)\n", buf, macs[i]);
		}
		else {
			if (strncmp(macs[i], failed_mac) == 0 || \
					strcmp(macs[i], "6C-F0-49-E0-95-9FA") == 0 || \
					strcmp(macs[i], "6C:F0:49:E0:95:9FA") == 0 || \
					strcmp(macs[i], "6CF04") == 0) {
				printf("OK: %s not found\n", macs[i]);
			}
			else {
				printf("FAIL: Fail to found %s (%d)\n", macs[i], res);
				exit(EXIT_FAILURE);
			}
		}
	}

	destroy_hash();
}
