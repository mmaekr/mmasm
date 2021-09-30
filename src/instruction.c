#include "instruction.h"

static void operand_init(operand* op) {
	op->optype = OP_NONE;
	op->value = 0;
	op->bits = 0;
	op->flags = 0;
}

instruction* inst_init() {
	instruction* inst = malloc(sizeof(instruction));
	inst->opcode = NULL;
	inst->no_operands = 0;
	inst->shift = 0;
	inst->extend = 0;
	for (int i = 0; i < MAX_OPERAND; i++) {
		operand_init(&inst->operands[i]);
	}
	return inst;
}

void inst_destroy(instruction* inst) {
	if (inst->opcode)
		free(inst->opcode);
	free(inst);
	inst = NULL;
}

void print_inst(instruction* inst) {
	char *type = NULL;
	printf("Opcode: %s\n", inst->opcode);
	for (int i = 0; i < MAX_OPERAND; i++)
	{
		switch(inst->operands[i].optype) {
			case OP_REG32:
				type = "32-bit Register";
				break;
			case OP_REG64:
				type = "64-bit Register";
				break;
			case OP_IMMEDIATE:
				type = "Immediate";
				break;
			case OP_LABEL:
				type = "Label";
				break;
			case OP_NONE:
			default:
				return;
		};
		if (type)
			printf("\tOperand #%d: %s -- %d, flags=%d\n", \
					i, type, inst->operands[i].value, inst->operands[i].flags);
	}
}

static void get_immediate(instruction* inst, int* imm) {
	for (int i = 0; i < MAX_OPERAND; i++) {
		if (inst->operands[i].optype == OP_IMMEDIATE)
			(*imm) = inst->operands[i].value;
		if (inst->operands[i].optype == OP_LABEL)
			(*imm) = inst->operands[i].value / 4;
	}
}

static void get_registers(instruction* inst, int* regs) {
	for (int i = 0; i < MAX_OPERAND; i++) {
		if (inst->operands[i].optype == OP_REG32 || \
				inst->operands[i].optype == OP_REG64) {
			regs[i] = inst->operands[i].value;
		}
	}
}

int gencode(instruction* inst) {
	// Extract fields from instruction
	int immediate = 0;
	get_immediate(inst, &immediate);
	int regs[MAX_OPERAND];
	get_registers(inst, regs);
	int shift = inst->shift;
	int extend = inst->extend;
	int sign = inst->sign;

	// out will contain the encoded instruction
	int cur_bits = INSTRUCTION_BITS;
	int out = 0;

	char* enc = strdup(insns_enc[inst->uid]);
#ifdef DEBUG
	printf("%s\n", enc);
#endif

	char* sp;
	char* tok = strtok_r(enc, " ", &sp);
	while (tok != NULL) {
		switch (tok[0]) {
			// sign bit
			case 's':
				cur_bits--;
				out |= sign << cur_bits;
				break;
			// literal
			case '%':
				int bits = strtol(strtok(tok, ".")+1, NULL, 16);
				int num = strtol(strtok(NULL, "."), NULL, 16);
#ifdef DEBUG
				printf("bits: %x, num: %x\n", bits, num);
#endif
				cur_bits -= bits;
				out |= num << cur_bits;
				break;
			// register
			case '$':
				int reg_n = strtol(tok+1, NULL, 10);
#ifdef DEBUG
				printf("register: %x\n", regs[reg_n]);
#endif
				cur_bits -= REG_BITS;
				out |= regs[reg_n] << cur_bits;
				break;
			// immediate
			case '#':
				int imm_bits = strtol(tok+1, NULL, 10);
#ifdef DEBUG
				printf("immediate: %d bits\n", imm_bits);
#endif
				cur_bits -= imm_bits;
				out |= immediate << cur_bits;
				break;
			// shift case
			case '<':
#ifdef DEBUG
				printf("shift: %d\n", shift);
#endif
				cur_bits -= SHIFT_BITS;
				out |= shift << cur_bits;
				break;
			// extend case
			case '>':
#ifdef DEBUG
				printf("extend: %d\n", extend);
#endif
				cur_bits -= EXT_BITS;
				out |= extend << cur_bits;
				break;

		};

#ifdef DEBUG
		printf("current bytes: 0x%4x\n", out);
#endif
		tok = strtok_r(NULL, " ", &sp);
	}

	free(enc);
	enc = NULL;
	return out;
}

int valid_inst(instruction* inst) {
	for (int i = 0; i < N_DEFINED_INSTRUCTIONS; i++) {
		if (compare_inst(*inst, insns_def[i])) {
			inst->sign = insns_def[i].sign;
			// unique id will be used to get the encoding
			inst->uid = i;
			return 1;
		}
	}
	return 0;
}

static int compare_operand(operand op_a, operand op_b) {
	if (op_a.flags != op_b.flags)
		return 0;
	return op_a.optype == op_b.optype;
}

int compare_inst(instruction inst_a, instruction inst_b) {
	int ret = 1;
	if (strcmp(inst_a.opcode, inst_b.opcode) == 0 && inst_a.no_operands == inst_b.no_operands) {
		for (int i = 0; i < inst_a.no_operands; i++) {
			if (!compare_operand(inst_a.operands[i], inst_b.operands[i])) {
				ret = 0;
				goto done;
			}
		}
	} else {
		ret = 0;
	}

done:
	return ret;
}

int check_inst(instruction* inst) {
	return inst->opcode != 0;
}
