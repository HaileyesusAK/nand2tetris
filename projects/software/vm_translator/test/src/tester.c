#include <linux/limits.h>
#include <stdio.h>
#include "test_utils.h"
#include "vm_translator.h"

int main () {

	int err = 0;
	char *input_dir = "../data/input";
	char *output_dir = "../data/result";
	char *expected_dir = "../data/expected";
	char * filenames[] = {"BasicTest", "PointerTest", "SimpleAdd",
						  "StackTest", "StaticTest"};
	char input_file[PATH_MAX], output_file[PATH_MAX], expected_file[PATH_MAX];

	void *vm_handle;

	printf("TESTING vm_translator.translate_asm_file\n");

	for(size_t i = 0; i < sizeof(filenames)/sizeof(char*) && !err; ++i)
	{
		
		snprintf(input_file, PATH_MAX, "%s.vm", filenames[i]);	
		vm_handle = init_vm_translator(input_file);
		if(!vm_handle)
		{
			fprintf(stderr, "error: cannot initialize VMTranslator\n");
			return -1;
		}

		snprintf(input_file, PATH_MAX, "%s/%s.vm", input_dir, filenames[i]);	
		snprintf(output_file, PATH_MAX, "%s/%s.asm", output_dir, filenames[i]);	
		snprintf(expected_file, PATH_MAX, "%s/%s.asm", expected_dir, filenames[i]);	
		
		FILE* in_fp = fopen(input_file, "r");
		if(!in_fp)
		{
			fprintf(stderr, "error: cannot open %s for reading\n", input_file);
			destroy_vm_translator(vm_handle);
			return -1;
		}
		
		FILE* out_fp = fopen(output_file, "w+");
		if(!out_fp)
		{
			fprintf(stderr, "error: cannot open %s for writing\n", output_file);
			fclose(in_fp);
			destroy_vm_translator(vm_handle);
			return -1;
		}

		
		FILE* expected_fp = fopen(expected_file, "r");
		if(!expected_fp)
		{
			fprintf(stderr, "error: cannot open %s for reading\n", expected_file);
			fclose(in_fp);
			fclose(out_fp);
			destroy_vm_translator(vm_handle);
			return -1;
		}
		
		err = translate_vm_file(vm_handle, in_fp, out_fp);
		if(err)
			fprintf(stderr, "error: translate_vm_file failed on %s\n", input_file);

		char msg[PATH_MAX];
		snprintf(msg, PATH_MAX, "comparing %s and %s", output_file, expected_file);
		assertEqualFile(out_fp, expected_fp,  msg);
		
		fclose(in_fp);
		fclose(out_fp);
		fclose(expected_fp);	
		destroy_vm_translator(vm_handle);
	}

	return err;
}
