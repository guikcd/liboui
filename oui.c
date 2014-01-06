#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "uthash.h"

#include "oui.h"

struct Manufacturer {
	char oui[OUI_LENGTH];
	char organization[ORGANIZATION_LENGTH];
	UT_hash_handle hh;
};

/* Contains all manufacturers with "oui" -> "organization name" values*/
struct Manufacturer *manufacturers = NULL;

/* Initialize the hash with "oui -> organization" values */
int create_hash() {
	regex_t re;
	size_t ngroups;
	regmatch_t *matches;
	/*size_t nmatched;*/
	int retval = 0;

	FILE *oui_file = fopen(oui_filename, "r");
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	char buf_oui[OUI_LENGTH], buf_organization[ORGANIZATION_LENGTH];


    if (regcomp(&re, LINE_TO_FIND, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex '%s'\n", LINE_TO_FIND);
		regfree(&re);
		return EXIT_FAILURE;
    }
	else {
		if (DEBUG == 1)
			fprintf(stdout, "Regex compiled\n");
		ngroups = re.re_nsub + 2;
		matches = malloc(ngroups * sizeof(regmatch_t));
	}

	if (!oui_file)
    {   
    	fprintf(stderr, "Error! Could not open file\n");
		fclose(oui_file);
		return EXIT_FAILURE;
	}
	else {
		if (DEBUG == 1)
			fprintf(stdout, "File opened\n");
	}

	while ((read = getline(&line, &len, oui_file)) != -1) {
		/* only get line that contain (hex) */
		if (DEBUG == 1)
			fprintf(stdout, "Analysing line '%s'\n", line);

		retval = regexec(&re, line, ngroups, matches, 0);
		/*if (DEBUG == 1)
			printf("Regex match ? %d\n", retval);*/
		if (retval == 0) {
			if (DEBUG == 1)
				fprintf(stdout, "Retrieved line of length %zu :\n", read);
			/*for (nmatched = 0; nmatched < ngroups; nmatched++) {*/
				/*printf("nmatched: %d, eo: %d, so: %d\n", (int)nmatched, matches[nmatched].rm_eo, matches[nmatched].rm_so);*/
				/* FIXME: trailing spaces */
				snprintf(buf_oui, sizeof(buf_oui), "%.*s", matches[1].rm_eo - matches[1].rm_so, &line[matches[1].rm_so]);
				snprintf(buf_organization, sizeof(buf_organization), "%.*s", matches[2].rm_eo - matches[2].rm_so, &line[matches[2].rm_so]);
				/* unwanted trailing \n */
				buf_organization[strnlen(buf_organization, ORGANIZATION_LENGTH)-1] = '\0';

				if (DEBUG == 1)
					fprintf(stdout, "OUI: %s, Org: %s\n", buf_oui, buf_organization);
				add_organization(buf_oui, buf_organization);
			/*}*/
		}
	}
	
	if (line)
	    free(line);

	free(matches);
	regfree(&re);
	fclose(oui_file);
	return EXIT_SUCCESS;
}

/* Get the organization (vendor) for the oui
 * The oui can be of the following form:
 * - ff:ff:ff:ff:ff:ff or FF:FF:FF:FF:FF:FF (full mac)
 * - FFFFFF or ffffff (just oui without two digit separator)
 * or
 * - ffffffffffff or FFFFFFFFFFFF (full mac without two digit separator)
 * - digit separator can be ":" or "-" (ex.: FF:FF:FF or FF-FF-FF)
 * Returns EXIT_SUCCESS or EXIT_FAILURE */
int get_organization(char *org, const char *oui) {
	int len;
	len = strlen(oui);

	/* only accept oui or full mac address */
	if (len == 8 || len == 17 || len == 6 || len == 12) {
		struct Manufacturer *entry;
		char normalized_oui[ORGANIZATION_LENGTH];
		char *just_oui = malloc(sizeof(oui));

		/* FF:FF:FF or ff:ff:ff (just oui with separator)
		 * or
		 * ff:ff:ff:ff:ff:ff or FF:FF:FF:FF:FF:FF (full mac)
		 * only the first 8 chars are interesting */
		if (len == 8 || len == 17) {
			just_oui[0] = oui[0];
			just_oui[1] = oui[1];
			just_oui[2] = oui[2];
			just_oui[3] = oui[3];
			just_oui[4] = oui[4];
			just_oui[5] = oui[5];
			just_oui[6] = oui[6];
			just_oui[7] = oui[7];
			just_oui[8] = '\0';
			normalize_oui(normalized_oui, just_oui);
		}
		/* FFFFFF or ffffff (just oui without two digit separator)
		 * or
		 * ffffffffffff or FFFFFFFFFFFF (full mac without two digit separator)
		 * only the first 8 chars are interesting, so ignore nic id */
		else {
		/*if (len == 6 || len == 12) {*/
			just_oui[0] = oui[0];
			just_oui[1] = oui[1];
			just_oui[2] = DASH;
			just_oui[3] = oui[2];
			just_oui[4] = oui[3];
			just_oui[5] = DASH;
			just_oui[6] = oui[4];
			just_oui[7] = oui[5];
			just_oui[8] = '\0';
			normalize_oui(normalized_oui, just_oui);
		}

		free(just_oui);

		if (DEBUG == 1)
			fprintf(stdout, "Normalized oui: %s (original %s)\n", normalized_oui, oui);

		HASH_FIND_STR(manufacturers, normalized_oui, entry);
		if (!entry) {
			return EXIT_FAILURE;
		}

		if (DEBUG == 1)
			fprintf(stdout, "Found entry %s for %s\n", entry->organization, normalized_oui);

		strncpy(org, entry->organization, ORGANIZATION_LENGTH);
		return EXIT_SUCCESS;
	}
	else {
		//fprintf(stderr, "%s is %d long (expected %d)\n", oui, len, OUI_LENGTH);
		return EXIT_FAILURE;
	}
}

/* Add oui and org to the hash
 * FIXME: why not free() "entry" ?*/
static void add_organization(const char *oui, const char *organization) {
	struct Manufacturer *entry;
	entry = malloc(sizeof(struct Manufacturer));
	strncpy(entry->oui, oui, OUI_LENGTH);
	strncpy(entry->organization, organization, ORGANIZATION_LENGTH);
	if (DEBUG == 1)
		fprintf(stdout, "Adding %s, %s\n", oui, organization);
	HASH_ADD_KEYPTR(hh, manufacturers, entry->oui, strnlen(entry->oui, OUI_LENGTH), entry);
}

/*
 * Normalize oui:
 * - ":" -> "-"
 * - all hexa letters uppercase
 *
 * ex.: 0a:0b:0c -> 0A-0B-0C
 * */
static void normalize_oui(char *new_oui, const char *oui) {
	/* strnlen() returns unsigned int (size_t) */
	unsigned int i;
	strncpy(new_oui, oui, OUI_LENGTH);

	/* uppercase hexa letters */
	for (i=0; i<=strnlen(oui, OUI_LENGTH); i++) {
		if( (oui[i] > 96 ) && (oui[i] < 123) )
			new_oui[i] = oui[i] - 'a' + 'A';
		else
			/* just copy the char as it is */
			new_oui[i] = oui[i];
	}

	/* replace ":" with "-" */
	if (new_oui[2] == (char) COLON && new_oui[5] == COLON) {
		new_oui[2] = DASH;
		new_oui[5] = DASH;
	}
}

/*
 * Destroy the hash containing oui -> organization
 */
void destroy_hash() {
	struct Manufacturer *current_manufacturer, *tmp;
	HASH_ITER(hh, manufacturers, current_manufacturer, tmp) {
		HASH_DEL(manufacturers, current_manufacturer);
		free(current_manufacturer);
	}
}

int main (void) {

	char my_oui[OUI_LENGTH] = "6c:F0:49";
	char buf[ORGANIZATION_LENGTH];
	int res;

	create_hash();

	res = get_organization(buf, my_oui);
	if (!res) {
		fprintf(stdout, "res(%d): %s\n", res, buf);
	}
	else {
		printf("Fail to found %s vendor!\n", my_oui);
	}

	char my_oui2[OUI_LENGTH] = "00:08:a1";
	res = get_organization(buf, my_oui2);
	if (!res) {
		printf("res(%d): %s\n", res, buf);
	}
	else {
		printf("Fail to found %s vendor!\n", my_oui2);
	}

	char my_oui3[OUI_LENGTH] = "FC-FE-77";
	res = get_organization(buf, my_oui3);
	if (!res) {
		printf("res(%d): %s\n", res, buf);
	}
	else {
		printf("Fail to found %s vendor!\n", my_oui3);
	}

	destroy_hash();

	exit(EXIT_SUCCESS);
}
