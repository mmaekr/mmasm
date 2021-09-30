#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

// Defines the instruction struct and operand struct
// Contains everything related to generating and validating instructions

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arm.h"

typedef enum {
	OP_NONE,
	OP_REG32,
	OP_REG64,
	OP_IMMEDIATE,
	OP_LABEL,
} operand_t;

enum { OPF_DEREF=1, OPF_SP=2 };

typedef struct{
	operand_t optype;
	int value;
	//TODO For immediates, when determining if they fit in
	// This will be necessary to do negative immediates as well
	int bits;
	// Bit fields for operand flags
	union {
		uint8_t flags;
		struct {
			unsigned int deref : 1;
			unsigned int sp : 1;
		};
	};

} operand;

typedef struct {
	char* opcode;
	operand operands[MAX_OPERAND];
	int no_operands;
	// for instructions that can be shifted with LSL, ASR, etc.
	int shift;
	// for instructions that can be extended...
	int extend;
	// determining if 32bit or 64bit instruction
	int sign;
	// unique ID that determines what encoding to use
	int uid;
} instruction;

extern int N_DEFINED_INSTRUCTIONS;
extern const instruction insns_def[];
extern const char* insns_enc[];

// initialize a new instructoin
instruction* inst_init(void);
// destroys an instruction
void inst_destroy(instruction*);
// generates the bytes using the instruction
int gencode(instruction*);
// checks if an instruction is valid by checking against a list of known instructions
int valid_inst(instruction*);
// compares two instructions, returns 1 if the are equal otherwise 0
int compare_inst(instruction, instruction);
// pretty prints an instruction
void print_inst(instruction*);
// check if an instruction actually exists thonk
int check_inst(instruction*);

#endif
