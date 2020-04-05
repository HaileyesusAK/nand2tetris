#ifndef __VM_TRANSLATOR_H__
#define __VM_TRANSLATOR_H__


#include <stdio.h>

void* init_vm_translator(char *static_prefix);
void destroy_vm_translator(void* handle);
int translate_vm_file(const void* handle, FILE* vm_file, FILE* asm_file);

#endif
