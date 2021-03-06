#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rtl_ini.h"

void create_example_ini_file(void);
int parse_ini_file(char *ini_name);

int main(int argc, char *argv[])
{
	int status;

	if (argc < 2) {
		create_example_ini_file();
		status = parse_ini_file("example.ini");
	} else {
		status = parse_ini_file(argv[1]);
	}
	return status;
}

void create_example_ini_file(void)
{
	FILE *ini;

	if ((ini = fopen("example.ini", "w")) == NULL) {
		fprintf(stderr, "ini: cannot create example.ini\n");
		return;
	}

	fprintf(ini,
			"#\n"
			"# This is an example of ini file\n"
			"#\n"
			"\n"
			"[Pizza]\n"
			"\n"
			"Ham       = yes ;\n"
			"Mushrooms = TRUE ;\n"
			"Capres    = 0 ;\n"
			"Cheese    = Non ;\n"
			"\n"
			"\n"
			"[Wine]\n"
			"\n"
			"Grape     = Cabernet Sauvignon ;\n"
			"Year      = 1989 ;\n"
			"Country   = Spain ;\n" "Alcohol   = 12.5  ;\n" "\n");
	fclose(ini);
}

int parse_ini_file(char *ini_name)
{
	rtl_dict_t *ini;

	/* Some temporary variables to hold query results */
	int b, i;
	double d;
	const char *s;

	ini = rtl_ini_load(ini_name);
	if (ini == NULL) {
		fprintf(stderr, "cannot parse file: %s\n", ini_name);
		return -1;
	}
	rtl_ini_dump(ini, stdout);
	rtl_ini_dump_ini(ini, stdout);

	/* Get pizza attributes */
	printf("Pizza has %d keys:\n", rtl_ini_get_sec_nkeys(ini, "pizza"));

	b = rtl_ini_get_boolean(ini, "pizza:ham", -1);
	printf("Ham:       [%d]\n", b);
	b = rtl_ini_get_boolean(ini, "pizza:mushrooms", -1);
	printf("Mushrooms: [%d]\n", b);
	b = rtl_ini_get_boolean(ini, "pizza:capres", -1);
	printf("Capres:    [%d]\n", b);
	b = rtl_ini_get_boolean(ini, "pizza:cheese", -1);
	printf("Cheese:    [%d]\n", b);

	/* Get wine attributes */
	printf("Wine:\n");
	s = rtl_ini_get_string(ini, "wine:grape", NULL);
	printf("Grape:     [%s]\n", s ? s : "UNDEF");

	i = rtl_ini_get_int(ini, "wine:year", -1);
	printf("Year:      [%d]\n", i);

	s = rtl_ini_get_string(ini, "wine:country", NULL);
	printf("Country:   [%s]\n", s ? s : "UNDEF");

	d = rtl_ini_get_double(ini, "wine:alcohol", -1.0);
	printf("Alcohol:   [%g]\n", d);

	rtl_ini_free_dict(ini);
	return 0;
}
