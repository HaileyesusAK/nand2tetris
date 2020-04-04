#ifndef __VM_TRANSLATOR_H__
#define __VM_TRANSLATOR_H__
#include <stdio.h>

#define RAM_SIZE 32768		//32K	
void* init_vm_translator(char *static_prefix);
void destroy_vm_translator(void* vm_translator);
int translate_vm_file(FILE* vm_file, FILE* asm_file);

#endif
