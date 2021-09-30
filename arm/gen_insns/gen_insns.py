#!/usr/bin/python3

import re
import sys

def init_encoding():
    enc_file = open('insns_enc.c', 'w')
    enc_file.write("// Automatically generated by rondine.py, do not touch :)\n\n")
    enc_file.write("const char* insns_enc[] = {\n")
    return enc_file

def init_hashtable():
    hash_file = open('insns_h.c', 'w')
    hash_file.write("// Automatically generated by rondine.py, do not touch :)\n\n")
    hash_file.write("#include \"insns_h.h\"\n\n")
    hash_file.write("struct hashtable* insns;\n\n")
    hash_file.write("static void insn_table_destroy() { return; }\n\n")
    hash_file.write("void exit_instructions() {\n")
    hash_file.write("\thashtable_destroy(insns);\n}\n\n")
    hash_file.write("void initialize_instructions() {\n")
    hash_file.write("\tinsns = hashtable_init(insn_table_destroy);\n")
    return hash_file

def init_definitions():
    def_file = open('insns_def.c', 'w')
    def_file.write("// Automatically generated by rondine.py, do not touch :)\n\n")
    def_file.write("#include \"instruction.h\"\n\n")
    def_file.write("const instruction insns_def[] = {\n")
    return def_file

def enc_action(enc_file, opcode, encoding):
    enc_file.write(f"\t\"{encoding}\", // {opcode}\n")

def hash_action(hash_file, opcode, count):
    hash_file.write(f"\t/* {hex(count)} */")
    hash_file.write(f"\thashtable_insert(insns, \"{opcode}\", (void*)1);\n")

def def_action(def_file, definition, count):
        opcode = definition[0]
        operands = []
        extend = 0
        shift = 0
        sign = 0
        deref = []
        sp = []
        max_operands = 5
        no_operands = 0
        for _ in range(max_operands):
            operands.append('OP_NONE')

        for _ in range(10):
            deref.append(0)
            sp.append(0)

        for j, operand in enumerate(definition[1:]):
            if operand.startswith('<W') or operand.startswith('<WSP'):
                operands[j] = 'OP_REG32'
                no_operands += 1
            elif operand.startswith('<X') or operand.startswith('<R') or operand.startswith('<SP'):
                operands[j] = 'OP_REG64'
                sign = 1
                no_operands += 1
            elif operand.startswith('#<imm') or operand.startswith('#<simm') or operand.startswith('#<pimm'):
                operands[j] = 'OP_IMMEDIATE'
                no_operands += 1
            elif operand.startswith('<shift'):
                shift = 1
                operands[j] = 'OP_IMMEDIATE'
                no_operands += 1
            elif operand.startswith('<extend'):
                extend = 1
                operands[j] = 'OP_IMMEDIATE'
                no_operands += 1
            elif operand.startswith('<label'):
                operands[j] = 'OP_LABEL'
                no_operands += 1
            elif operand.startswith('['):
                deref[j] = 1
                for aa in range(max_operands-j):
                    deref[j+aa] = 1
                operand = operand[1:]
                if operand.startswith('<W') or operand.startswith('<WSP'):
                    operands[j] = 'OP_REG32'
                    no_operands += 1
                elif operand.startswith('<X') or operand.startswith('<SP'):
                    operands[j] = 'OP_REG64'
                    sign = 1
                    no_operands += 1
                elif operand.startswith('#<imm') or operand.startswith('#<simm') or operand.startswith('#<amount') or operand.startswith('#<pimm'):
                    operands[j] = 'OP_IMMEDIATE'
                    no_operands += 1
                elif operand.startswith('<shift'):
                    shift = 1
                    operands[j] = 'OP_IMMEDIATE'
                    no_operands += 1
                elif operand.startswith('<extend'):
                    extend = 1
                    operands[j] = 'OP_IMMEDIATE'
                    no_operands += 1
                elif operand.startswith('<label'):
                    operands[j] = 'OP_LABEL'
                    no_operands += 1

            else:
                pass

            if ']' in operand:
                for aa in range(max_operands-j):
                    deref[j+1+aa] = 0
            if 'SP' in operand:
                sp[j] = 1


        s = "{" + f"\"{opcode}\", " + "{"
        for k, operand in enumerate(operands):
            flags = []
            if deref[k]:
                flags.append("OPF_DEREF")
            if sp[k]:
                flags.append("OPF_SP")
            s += "{" + f"{operand}, 0, 0, {{{'0' if not flags else '|'.join(str(f) for f in flags)}}}" + "},"
        s += "}," + f" {no_operands}, {shift}, {extend}, {sign}, 0" + "}"
        def_file.write(f"\t/* {hex(count)} */")
        def_file.write("\t" + s + ",\n")


def main():
    enc_file = init_encoding()
    hash_file = init_hashtable()
    def_file = init_definitions()
    with open(sys.argv[1], 'r') as inst_dat:
        count = 0
        for line in inst_dat:
            data = line.strip().split('\t\t')
            definition = data[0].split(' ')
            encoding = ""
            if len(data) > 1:
                encoding = data[1]
            opcode = definition[0]

            def_action(def_file, definition, count)
            enc_action(enc_file, opcode, encoding)
            hash_action(hash_file, opcode, count)
            count += 1
        enc_file.write("};")
        hash_file.write("};")
        def_file.write("};\n\n")

        def_file.write(f"int N_DEFINED_INSTRUCTIONS = {count};")

if __name__ == "__main__":
    main()