#include<stdio.h>
#include<limits.h>
#include<string.h>

#include "vm_translator.h"

int main(int argc, char* argv[])
{

	if(argc != 2)
	{
		fprintf(stderr, "usage: %s <vm file>\n", argv[0]);
		return -1;
	}

	FILE* vm_file = fopen(argv[1], "r");
	if(!vm_file)
	{
		fprintf(stderr, "cannot open %s for reading\n", argv[1]);
		return -1;
	}

	char asm_file_path[PATH_MAX + 4] = {0};	//+4 for the extension .asm
	snprintf(asm_file_path, PATH_MAX - 1, "%s", argv[1]);
	char *ext = strrchr(asm_file_path, '.');
	if(ext && ext != asm_file_path) {
		strcpy(ext, ".asm");}
	else
		strcat(asm_file_path, ".asm");

	FILE* asm_file = fopen(asm_file_path, "w");
	if(!asm_file)
	{
		fclose(vm_file);
		fprintf(stderr, "cannot open %s for writing\n", asm_file_path);
		return -1;
	}

	char *asm_file_name = strrchr(asm_file_path, '/');
	if(asm_file_name)
		asm_file_name++;
	else
		asm_file_name = asm_file_path;

	void *handle = init_vm_translator(asm_file_name);
	if(!handle)
	{
		fprintf(stderr, "Failed to initialized vm translator\n");
		fclose(vm_file);
		fclose(asm_file);
		return -1;
	}	
	
	int rc = translate_vm_file(handle, vm_file, asm_file);
	if(rc)
		fprintf(stderr, "Failed to translate %s\n", argv[1]);

	fclose(vm_file);
	fclose(asm_file);
	destroy_vm_translator(handle);
	return rc;
}
