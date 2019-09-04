#ifndef OUI_H
#define OUI_H

#define DEBUG 0

/* ":" */
#define COLON 58
/* "-" */
#define DASH 45

/* ex.: "707E43" */
#define OUI_LENGTH 7

/* ex.: XEROX CORPORATION */
#define ORGANIZATION_LENGTH 50

#define MAC_ADDRESS_LENGTH 17

/* ex.: "00-00-00   (hex)      XEROX CORPORATION" */
#define LINE_TO_FIND "[[:space:]]*([0-9A-F]{6})[[:space:]]*\\(base.16\\)[[:space:]]*(.*)"

const char *oui_filename = "oui.txt";
/*
  00-00-00   (hex)		XEROX CORPORATION
  000000     (base 16)		XEROX CORPORATION
  				M/S 105-50C
				800 PHILLIPS ROAD
				WEBSTER NY 14580
				UNITED STATES
*/

/* Initialize the hash with "oui -> organization" values
 * ex.: 00-00-00 -> XEROX CORPORATION */
int create_hash();

/* Get the organization (vendor) for the oui
 * The oui can be of the following form:
 * - ff:ff:ff:ff:ff:ff or FF:FF:FF:FF:FF:FF (full mac)
 * - FF:FF:FF or ff:ff:ff (just oui)
 * - digit separator can be ":" or "-" (ex.: FF:FF:FF or FF-FF-FF)
 * Returns EXIT_SUCCESS or EXIT_FAILURE
 * FIXME: bug when calling the function twice with the same oui */
int get_organization(char *org, const char *oui);

void destroy_hash();

static void normalize_oui(char *new_oui, const char *oui);
static void add_organization(const char *oui, const char *organization);

#endif
