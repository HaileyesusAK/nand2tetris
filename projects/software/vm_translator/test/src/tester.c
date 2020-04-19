#include <linux/limits.h>
#include <stdio.h>

#include "test_utils.h"
#include "vm_translator.h"

int main () {

	int err = 0;
	char *input_dir = "../data/input";
	char *output_dir = "../data/result";
	char *expected_dir = "../data/expected";
	char *filenames[] = {"BasicTest.vm", "PointerTest.vm", "SimpleAdd.vm",
						 "StackTest.vm", "StaticTest.vm", "FibonacciElement",
						 "StaticsTest"};
	char input_file[PATH_MAX], output_file[PATH_MAX], expected_file[PATH_MAX];

	printf("TESTING vm_translator.translate_asm_file\n");

	for(size_t i = 0; i < sizeof(filenames)/sizeof(char*) && !err; ++i)
	{
		if(strstr(filenames[i], ".vm"))
		{
			size_t len;
			snprintf(input_file, PATH_MAX, "%s/%s", input_dir, filenames[i]);

			len = snprintf(output_file, PATH_MAX, "%s/%s", output_dir, filenames[i]);
			snprintf(&output_file[len-3], PATH_MAX-len, ".asm");

			len = snprintf(expected_file, PATH_MAX, "%s/%s", expected_dir, filenames[i]);
			snprintf(&expected_file[len-3], PATH_MAX-len, ".asm");
		}
		else
		{
			snprintf(input_file, PATH_MAX, "%s/%s", input_dir, filenames[i]);
			snprintf(output_file, PATH_MAX, "%s/%s.asm", output_dir, filenames[i]);
			snprintf(expected_file, PATH_MAX, "%s/%s.asm", expected_dir, filenames[i]);
		}

		err = translate_vm(input_file, output_file);
		if(err)
		{
			fprintf(stderr, "error: translate_vm_file failed on %s\n", input_file);
			continue;
		}

		FILE* out_fp = fopen(output_file, "r");
		if(!out_fp)
		{
			fprintf(stderr, "error: cannot open %s for reading\n", output_file);
			return -1;
		}


		FILE* expected_fp = fopen(expected_file, "r");
		if(!expected_fp)
		{
			fprintf(stderr, "error: cannot open %s for reading\n", expected_file);
			fclose(out_fp);
			return -1;
		}

		char msg[PATH_MAX];
		snprintf(msg, PATH_MAX, "comparing %s and %s", output_file, expected_file);
		assertEqualFile(out_fp, expected_fp,  msg);

		fclose(out_fp);
		fclose(expected_fp);
	}

	return err;
}
