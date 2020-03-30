#ifndef __VM_TRANSLATOR_H__
#define __VM_TRANSLATOR_H__
#include <stdio.h>

#define RAM_SIZE 32768		//32K	
void* init_vm_translator();
int translate_vm_inst(const char* vm_inst, char* asm_inst, size_t* inst_len);
int translate_vm_file(FILE* vm_file, FILE* asm_file);

#endif
