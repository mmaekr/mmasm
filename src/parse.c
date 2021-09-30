#include "parse.h"

int is_instruction(const char* str) {
	if (insns) {
		return (long) hashtable_get(insns, (char*)str);
	} else {
		printf("Instruction table not loaded?! Fatal!\n");
		exit(-1);
	}
}

static int is_label_def(const char* str) {
	// Label definitions must end with ':'
	if (str[strlen(str)-1] == ':')
		return 1;
	return 0;
}

static int is_reg32(const char* str) {
	// Check for special registers
	if (strcmp(str, "wsp") == 0)
		return 1;
	if (strcmp(str, "wzr") == 0)
		return 1;

	// Registers start with 'r'
	if (str[0] == 'w') {
		char* endptr = NULL;
		strtol(str+1, &endptr, 10);
		if (endptr > str+1 && endptr[0] == '\0')
			return 1;
	}
	return 0;
}

static int is_reg64(const char* str) {
	// Check for special registers
	if (strcmp(str, "lr") == 0)
		return 1;
	if (strcmp(str, "sp") == 0)
		return 1;
	if (strcmp(str, "xzr") == 0)
		return 1;
	
	// Registers start with 'r'
	if (str[0] == 'x') {
		char* endptr = NULL;
		strtol(str+1, &endptr, 10);
		if (endptr > str+1 && endptr[0] == '\0')
			return 1;
	}
	return 0;
}

static int is_regsp(const char* str) {
	return strstr(str, "sp") ? 1 : 0;
}

static int is_immediate(const char* str) {
	// All immediates must start with '#'
	if (str[0] == '#') {
		char* endptr = NULL;
		int v = 0;

		// Check if immediate is negative
		if (str[1] == '-') {
			str++;
		}

		// Check for binary immediate
		if (strncmp(str+1, "0b", 2) == 0) {
			v = (int)strtol(str+3, &endptr, 2);
			if (endptr > str+3 && endptr[0] == '\0')
				return 1;
		// Check for hex immediate
		} else if (strncmp(str+1, "0x", 2) == 0) {
			v = (int)strtol(str+3, &endptr, 16);
			if (endptr > str+3 && endptr[0] == '\0')
				return 1;
		// Check for decimal immediate
		} else {
			v = (int)strtol(str+1, &endptr, 10);
			if (endptr > str+1 && endptr[0] == '\0')
				return 1;
		}
		v += 0;
	}
	return 0;
}

static char* normalize_string(const char* str) {
	size_t len = strlen(str);
	char* lower = calloc(len+1, sizeof(char));
	for (size_t i = 0; i < len; i++) {
		lower[i] = tolower(str[i]);
	}

	return lower;
}

static int is_shift(const char* str) {
	// There are four different types of shifts: ASR, LSL, LSR, ROR
	if (strcmp(str, "asr") == 0 || strcmp(str, "asr") == 0 ||
			strcmp(str, "lsr") == 0 || strcmp(str, "ror") == 0) {
		return 1;
	}
	return 0;
}

static int is_extend(const char* str) {
	// There are eight different types of extensions: UXTB, UXTH, LSL, UXTX, SXTB, SXTH, SXTW, SXTX
	if (strcmp(str, "uxtb") == 0 || strcmp(str, "uxth") == 0 ||
			strcmp(str, "lsl") == 0 || strcmp(str, "uxtx") == 0 ||
			strcmp(str, "sxtb") == 0 || strcmp(str, "sxth") == 0 ||
			strcmp(str, "sxtw") == 0 || strcmp(str, "sxtx") == 0) {
		return 1;
	}
	return 0;
}

static int is_label(const char* str, symtab* symbol_table) {
	// Check if string exists in symbol table
	if (symtab_contains_key(symbol_table, str))
		return 1;
	return 0;
}

static int is_comment(const char* str) {
	// Comments begin with ;
	if (str[0] == ';')
		return 1;
	return 0;
}

static int is_s_deref(const char* str) {
	// Checking for square brackets
	return str[0] == '[';
}

static int is_e_deref(const char* str) {
	return strchr(str, ']') ? 1 : 0;
}

static void* get_e_deref(const char* str) {
	return strchr(str, ']');
}

static const char* get_symbol_name(char* str) {
	// If label definition, strip ':' -- return symbol name
	if (str[strlen(str)-1] == ':')
		str[strlen(str)-1] = '\0';
	return str;
}

static int get_register(const char* str) {
	// Check if we have 'SP' register
	if (strcmp(str, "sp") == 0 || strcmp(str, "wsp") == 0 ||
			strcmp(str, "wzr") == 0 || strcmp(str, "xzr") == 0)
		return SP_REG_NO;
	if (strcmp(str, "lr") == 0)
		return LR_REG_NO;

	// Skip 'x' and 'w' character in register
	char* endptr = NULL;
	int v = (int)strtol(str+1, &endptr, 10);
	if (endptr > str+1 && endptr[0] == '\0') {
 		if (v <= REGISTER_NO)
			return v;
		else
			return -1;
	}
	return -2;
}

static int get_immediate_value(const char* str) {
	char* endptr = NULL;
	int v = 0;

	// Multiply by 1 if positive, 0 if negative
	int neg = 1;
	// Check if immediate is negative
	if (str[1] == '-') {
		str++;
		neg = -1;
	}

	// Check for binary immediate
	if (strncmp(str+1, "0b", 2) == 0) {
		v = (int)strtol(str+3, &endptr, 2);
		if (endptr > str+3 && endptr[0] == '\0')
			return v * neg;
	// Check for hex immediate
	} else if (strncmp(str+1, "0x", 2) == 0) {
		v = (int)strtol(str+3, &endptr, 16);
		if (endptr > str+3 && endptr[0] == '\0')
			return v * neg;
	// Check for decimal immediate
	} else {
		v = (int)strtol(str+1, &endptr, 10);
		if (endptr > str+1 && endptr[0] == '\0')
			return v * neg;
	}
	return -1;
}

static int get_shift(const char* str) {
	// Shifts in ARM are encoded in 2 bits
	int ret = 0;
	if (strcmp(str, "asr") == 0)
		ret = SHIFT_ASR;
	else if (strcmp(str, "lsl") == 0)
		ret = SHIFT_LSL;
	else if (strcmp(str, "lsr") == 0)
		ret = SHIFT_LSR;
	else if (strcmp(str, "ror") == 0)
		ret = SHIFT_ROR;
	else
		ret = -1;

	return ret;
}

static int get_extend(const char* str) {
	// Extensions in ARM are encoded in 3 bits
	int ret = 0;
	if (strcmp(str, "uxtb") == 0)
		ret = EXT_UXTB;
	else if (strcmp(str, "uxth") == 0)
		ret = EXT_UXTH;
	else if (strcmp(str, "lsl") == 0)
		ret = EXT_LSL;
	else if (strcmp(str, "uxtx") == 0)
		ret = EXT_UXTX;
	else if (strcmp(str, "sxtb") == 0)
		ret = EXT_SXTB;
	else if (strcmp(str, "sxth") == 0)
		ret = EXT_SXTH;
	else if (strcmp(str, "sxtw") == 0)
		ret = EXT_SXTW;
	else if (strcmp(str, "sxtx") == 0)
		ret = EXT_SXTX;
	else
		ret = -1;
	return ret;
}

static int get_label_value(const char* str, symtab* symbol_table) {
	return symtab_get(symbol_table, str);
}

// parse_line is the real meat of the parsing
int parse_line(char* raw_line, struct pass_ctx* ctx) {
	int err = 0;
	int i = 0;
	int comment = 0;
	char* s = NULL;

	ctx->line_number++;

	// Only work with lowercase characters for parsing
	char* line = normalize_string(raw_line);

	char* newline = strchr(line, '\n');
	if (newline)
		*newline = 0;
	if (line[0] == '\0') {
		free(line);
		line = NULL;
		return 0;
	}

	// Build the instruction, only if we are in pass 2
	instruction* inst = NULL;
	if (ctx->pass == SECOND_PASS)
		inst = inst_init();

	s = strtok(line, ", \t\n");

	// Generate instruction structure in second pass
	while (s != NULL && !comment) {
		switch (i) {
			case 0:
				if (is_comment(s)) {
#ifdef DEBUG
					printf("COMMENT\n");
#endif
					comment = 1;
					break;

				// If string ends in ":", we define it as a label
				} else if (is_label_def(s)) {
#ifdef DEBUG
					printf("LABEL: %s\n", s);
#endif
					// First pass adds labels to symbol table
					if (ctx->pass == FIRST_PASS) {
						symtab_set(ctx->symbol_table, get_symbol_name(s), ctx->address);
					}
					
				} else if (is_instruction(s)) {
#ifdef DEBUG
					printf("INSTRUCTION: %s\n", s);
#endif
					if(ctx->pass == SECOND_PASS)
						inst->opcode = strdup(s);

					ctx->address += INSTRUCTION_SIZE;
				} else {
					error_ss_line(raw_line, s, "Unknown instruction!", ctx->line_number);
					err = 1;
					goto exit;
				}
				break;

			// Operand parsing
			case 1:
			case 2:
			case 3:
			case 4:
				
				if (is_s_deref(s)) {
					if (ctx->pass == SECOND_PASS) {
						for (int j = 0; j < MAX_OPERAND-i; j++) {
							inst->operands[i-1+j].deref = 1;
						}
					}
					s = s+1;
				}
				
				if (is_e_deref(s)) {
					if (ctx->pass == SECOND_PASS) {
						for (int j = 0; j < MAX_OPERAND-i; j++) {
							inst->operands[i+1+j].deref = 0;
						}
					}
					// we need to remove the ] here...
					char* e_deref = (char*)get_e_deref(s);
					*e_deref = '\0';
				}


				if (is_comment(s)) {
#ifdef DEBUG
					printf("COMMENT\n");
#endif
					comment = 1;
					break;
				}
				else if (is_reg32(s)) {
#ifdef DEBUG
					printf("REG32: %s\n", s);
#endif
					if (ctx->pass == SECOND_PASS) {
						inst->operands[inst->no_operands].optype = OP_REG32;
						int v = get_register(s);
						switch (v) {
							case -1:
								error_ss_line(raw_line, s, "Unknown register!", ctx->line_number);
								err = 1;
								goto exit;
							case -2:
								err = 1;
								goto exit;
							default:
								if (v == SP_REG_NO && is_regsp(s))
									inst->operands[inst->no_operands].sp = 1;
								inst->operands[inst->no_operands++].value = v;
						};
					}
				} else if (is_reg64(s)) {
#ifdef DEBUG
					printf("REG64: %s\n", s);
#endif
					if (ctx->pass == SECOND_PASS) {
						inst->operands[inst->no_operands].optype = OP_REG64;
						int v = get_register(s);
						switch (v) {
							case -1:
								error_ss_line(raw_line, s, "Unknown register!", ctx->line_number);
								err = 1;
								goto exit;
							case -2:
								err = 1;
								goto exit;
							default:
								if (v == SP_REG_NO && is_regsp(s))
									inst->operands[inst->no_operands].sp = 1;
								inst->operands[inst->no_operands++].value = v;
						};
					}
				} else if (is_immediate(s)) {
#ifdef DEBUG
					printf("immediate: %s\n", s);
#endif
					if (ctx->pass == SECOND_PASS) {
						inst->operands[inst->no_operands].optype = OP_IMMEDIATE;
						int v = get_immediate_value(s);
						if (v != -1)
							inst->operands[inst->no_operands++].value = v;
						else {
							printf("Unknown immediate! %s\n", s);
							err = 1;
							goto exit;
						}
					}
				} else if (is_shift(s) || is_extend(s)) {
#ifdef DEBUG
					printf("SHIFT/EXTEND: %s\n", s);
#endif
					if (ctx->pass == SECOND_PASS) {
						int v = get_shift(s);
						if (v >= 0) {
							inst->shift = v;
							i--;	// shifts are a psuedo operand, so don't make this null
						}
						else {
							int v = get_extend(s);
							if (v >= 0) {
								inst->extend = v;
								i--;
							} else {
								printf("Unknown shift/extend! %s\n", s);
								err = 1;
								goto exit;
							}
						}
					}
				} else {
					if (ctx->pass == SECOND_PASS) {
						if (is_label(s, ctx->symbol_table)) {
#ifdef DEBUG
							printf("LABEL: %s\n", s);
#endif
							inst->operands[inst->no_operands].optype = OP_LABEL;
							int v = get_label_value(s, ctx->symbol_table);
							if (v >= 0)
								inst->operands[inst->no_operands++].value = v;
							else {
								printf("Unknown label! %s\n", s);
								err = 1;
								goto exit;
							}
						} else {
							error_ss_line(raw_line, s, "Unknown value!", ctx->line_number);
							err = 1;
							goto exit;
						}
					}
				}

				break;
		};
		s = strtok(NULL, ", \t\n");
		i++;
	}
	
	if (ctx->pass == SECOND_PASS && check_inst(inst)) {
#ifdef DEBUG
		print_inst(inst);
#endif
		// If the instruction is valid, build it, otherwise error out
		if (valid_inst(inst)) {
#ifdef DEBUG
			printf("instruction unique id: %d\n", inst->uid);
#endif
			int x = gencode(inst);
			fwrite(&x, sizeof(int), 1, ctx->out_fd);
		} else {
			error_line(raw_line, "Bad instruction/opcode!", ctx->line_number);
			err = 1;
			goto exit;
		}
	}

exit:
	if (ctx->pass == SECOND_PASS)
		inst_destroy(inst);
	free(line);
	line = NULL;
	return err;
}
