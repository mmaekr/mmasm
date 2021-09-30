#include "error.h"

void error_line(char* raw_line, char* err, int line) {
	if (raw_line[strlen(raw_line)-1] == '\n')
		raw_line[strlen(raw_line)-1] = '\0';
	printf("%sError on line %d:%s %s\n\t%s\n", SBLD, line, SNBLD, raw_line, err);
}

void error_ss_line(char* raw_line, char* ss, char* err, int line) {
	int MAX_UNDERLINE_LENGTH = 0x40;

	if (raw_line[strlen(raw_line)-1] == '\n')
		raw_line[strlen(raw_line)-1] = '\0';

	int idx = strstr(raw_line, ss) - raw_line;
	int len = strlen(ss);
	if (len >= MAX_UNDERLINE_LENGTH)
		len = MAX_UNDERLINE_LENGTH;

	int u_len = 0;
	int raw_line_len = strlen(raw_line);
	char* underline = strdup(raw_line);
	for (int i = 0; i < raw_line_len; i++) {
		if (i >= idx && u_len < len) {
			underline[i] = '^';
			u_len++;
			continue;
		}
		if (underline[i] >= 0x21 && underline[i] <= 0x7e)
			underline[i] = ' ';
	}

	printf("%sError on line %d:%s %s%s%s\n\t%s\n", SBLD, line, SNBLD, CRED, err, CNRM, raw_line);
	printf("\t%s%s%s\n", SBLD, underline, SNBLD);
	free(underline);
	underline = NULL;
}
