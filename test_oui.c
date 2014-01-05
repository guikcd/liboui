#include <stdlib.h>
#include <stdio.h>
#include "oui.h"

int main (void) {
	char buf[ORGANIZATION_LENGTH];
	int res = 0;

	create_hash();

	/* virtually generated, not found in database */
	const char *failed_mac = "52:54:00:8d:b8:20";

	const char *macs[12];
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
	int i;

	for (i; i < sizeof(macs) / sizeof(*macs); i++) {
		/*printf("%s\n", macs[i]);*/
		res = get_organization(buf, macs[i]);
		if (res == 0) {
			printf("OK: %s (%s)\n", buf, macs[i]);
		}
		else {
			if (macs[i] != failed_mac) {
				printf("FAIL: Fail to found %s (%d)\n", macs[i], res);
				exit(EXIT_FAILURE);
			}
			else {
				printf("OK: %s not found\n", macs[i]);
			}
		}
	}

	destroy_hash();
}
