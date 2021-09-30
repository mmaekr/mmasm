#ifndef _PARSE_H_
#define _PARSE_H_

// Defines our one parsing function

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arm.h"
#include "error.h"
#include "insns_h.h"
#include "instruction.h"
#include "pass.h"
#include "symtab.h"

extern struct hashtable* insns;

// All the parsing is handled in here
int parse_line(char*, struct pass_ctx*);

#endif
