#ifndef _PASS_H_
#define _PASS_H_

// Defines the one pass function, handles everything

#include "arm.h"
#include "symtab.h"

#define FIRST_PASS 0
#define SECOND_PASS 1

struct pass_ctx {
	int pass;
	symtab* symbol_table;
	int address;
	int line_number;
	FILE* out_fd;
};

// Attempts to assemble a file and output to file name
int handle_file(const char*, const char*);

#endif
