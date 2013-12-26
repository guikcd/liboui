#ifndef OUI_H
#define OUI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "uthash.h"

#define DEBUG 0

/* ":" */
#define COLON 58
/* "-" */
#define DASH 45

/* ex.: "70-7E-43" */
#define OUI_LENGTH 9

/* ex.: XEROX CORPORATION */
#define ORGANIZATION_LENGTH 30

/* ex.: "00-00-00   (hex)      XEROX CORPORATION" */
#define LINE_TO_FIND "[[:space:]]*(.*)[[:space:]]*\\(hex\\)[[:space:]]*(.*)"

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
void create_hash();

/* Get the organization (vendor) for the oui */
int get_organization(char org[ORGANIZATION_LENGTH], const char oui[OUI_LENGTH]);

void destroy_hash();

static void normalize_oui(char new_oui[OUI_LENGTH], const char oui[OUI_LENGTH]);
static void add_organization(const char oui[OUI_LENGTH], const char organization[ORGANIZATION_LENGTH]);

#endif
