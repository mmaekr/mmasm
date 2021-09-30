CC=gcc
EXE=mmasm
ARM_DIR=arm
GENINSNS_DIR=gen_insns
INCLUDE_DIR=include
SRC_DIR=src
OBJ_DIR=build
INSN_DAT=$(GENINSNS_DIR)/instructions.dat
GENINSNS_SCRIPT=gen_insns.py

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC:%.c=%.o))
INSNS_OBJ:= $(ARM_DIR)/insns_def.o $(ARM_DIR)/insns_enc.o $(ARM_DIR)/insns_h.o

C_FLAGS:=-Wall -I $(INCLUDE_DIR)

all: build_dir insns $(EXE)

debug: C_FLAGS += -D DEBUG -g
debug: all

insns:
	cd arm; python $(GENINSNS_DIR)/$(GENINSNS_SCRIPT) $(INSN_DAT)
	$(MAKE) -C $(ARM_DIR)

build_dir:
	@mkdir -p $(OBJ_DIR)

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(INSNS_OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $? $(C_FLAGS) -o $@

.PHONY: clean
clean:
	-@rm -r $(OBJ_DIR) 2>/dev/null
	-@rm $(EXE) 2>/dev/null
	-@rm a.out 2>/dev/null
	-@make -C $(ARM_DIR) clean
