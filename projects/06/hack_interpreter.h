#ifndef __HACK_INTERPRETER_H__
#define __HACK_INTERPRETER_H__

int interpret_asm(FILE *asm_file, void *symbol_table, FILE *output_file);

#endif
