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
			printf("Regex compiled\n");
		ngroups = re.re_nsub + 2;
		matches = malloc(ngroups * sizeof(regmatch_t));
	}

	if (!oui_file)
    {   
    	printf("Error! Could not open file\n");
		fclose(oui_file);
		return EXIT_FAILURE;
	}
	else {
		if (DEBUG == 1)
			printf("File opened\n");
	}

	while ((read = getline(&line, &len, oui_file)) != -1) {
		/* only get line that contain (hex) */
		if (DEBUG == 1)
			printf("Analysing line '%s'\n", line);

		retval = regexec(&re, line, ngroups, matches, 0);
		/*if (DEBUG == 1)
			printf("Regex match ? %d\n", retval);*/
		if (retval == 0) {
			if (DEBUG == 1)
				printf("Retrieved line of length %zu :\n", read);
			/*for (nmatched = 0; nmatched < ngroups; nmatched++) {*/
				/*printf("nmatched: %d, eo: %d, so: %d\n", (int)nmatched, matches[nmatched].rm_eo, matches[nmatched].rm_so);*/
				/* FIXME: trailing spaces */
				snprintf(buf_oui, sizeof(buf_oui), "%.*s", matches[1].rm_eo - matches[1].rm_so, &line[matches[1].rm_so]);
				snprintf(buf_organization, sizeof(buf_organization), "%.*s", matches[2].rm_eo - matches[2].rm_so, &line[matches[2].rm_so]);
				/* unwanted trailing \n */
				buf_organization[strlen(buf_organization)-1] = '\0';

				if (DEBUG == 1)
					printf("OUI: %s, Org: %s\n", buf_oui, buf_organization);
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
 * Returns EXIT_SUCCESS or EXIT_FAILURE
 * FIXME: bug when calling the function twice with the same oui
 */
int get_organization(char org[ORGANIZATION_LENGTH], const char oui[OUI_LENGTH]) {
	struct Manufacturer *entry;
	entry = malloc(sizeof(struct Manufacturer));
	char normalized_oui[ORGANIZATION_LENGTH];

	/* oui are stored like 00-00-1A */
	normalize_oui(normalized_oui, oui);
	if (DEBUG == 1)
		printf("Normalized oui: %s (original %s)\n", normalized_oui, oui);

	HASH_FIND_STR(manufacturers, normalized_oui, entry);
	if (!entry) {
		free(entry);
		return EXIT_FAILURE;
	}

	if (DEBUG == 1)
		printf("Found entry %s for %s\n", entry->organization, normalized_oui);

	strncpy(org, entry->organization, ORGANIZATION_LENGTH);
	free(entry);
	return EXIT_SUCCESS;
}

/* Add oui and org to the hash
 * FIXME: why not free() "entry" ?*/
static void add_organization(const char oui[OUI_LENGTH], const char organization[ORGANIZATION_LENGTH]) {
	struct Manufacturer *entry;
	entry = malloc(sizeof(struct Manufacturer));
	strncpy(entry->oui, oui, OUI_LENGTH);
	strncpy(entry->organization, organization, ORGANIZATION_LENGTH);
	if (DEBUG == 1)
		printf("Adding %s, %s\n", oui, organization);
	HASH_ADD_KEYPTR(hh, manufacturers, entry->oui, strlen(entry->oui), entry);
}

/*
 * Normalize oui:
 * - ":" -> "-"
 * - all hexa letters uppercase
 *
 * ex.: 0a:0b:0c -> 0A-0B-0C
 * */
static void normalize_oui(char new_oui[OUI_LENGTH], const char oui[OUI_LENGTH]) {
	/* strlen() returns unsigned int (size_t) */
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
		printf("res(%d): %s\n", res, buf);
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
