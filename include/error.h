#ifndef _ERROR_H_
#define _ERROR_H_

// Defines functions for error handling, oooh pretty colors wow

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define SBLD   "\033[1m"
#define SNBLD  "\033[22m"

// Displays line error with message
void error_line(char*, char*, int);
// Displays line error with substring underlined and message
void error_ss_line(char*, char*, char*, int);

#endif
