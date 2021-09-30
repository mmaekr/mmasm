#ifndef _ARM_H_
#define _ARM_H_

// Defines random ARM related values

#define INSTRUCTION_BITS 32
#define REGISTER_NO 31
#define INSTRUCTION_SIZE 4
#define MAX_OPERAND 5
#define REG_BITS 5

#define LR_REG_NO 30
#define SP_REG_NO 31

#define SHIFT_BITS 2
#define SHIFT_ASR 0b10
#define SHIFT_LSL 0b00
#define SHIFT_LSR 0b01
#define SHIFT_ROR 0b11

#define EXT_BITS 3
#define EXT_UXTB 0b000
#define EXT_UXTH 0b001
#define EXT_LSL  0b010
#define EXT_UXTX 0b011
#define EXT_SXTB 0b100
#define EXT_SXTH 0b101
#define EXT_SXTW 0b110
#define EXT_SXTX 0b111

#endif
