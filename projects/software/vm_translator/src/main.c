#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "vm_translator.h"

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "usage: %s <input file or folder>\n", argv[0]);
		return -1;
	}

	char output_path[PATH_MAX];

	struct stat sb;
	if(stat(argv[1], &sb))
	{
		fprintf(stderr, "%s: failed to stat '%s' at %s, line %d\n",
						__func__, argv[1], __FILE__, __LINE__);
		return -1;
	}

	if((sb.st_mode & S_IFMT) == S_IFREG)
	{
		size_t len = snprintf(output_path, PATH_MAX, "%s", argv[1]);
		snprintf(&output_path[len - 3], PATH_MAX - len, ".asm");
	}
	else if((sb.st_mode & S_IFMT) == S_IFDIR)
	{

		snprintf(output_path, PATH_MAX, "%s/%s.asm", argv[1], basename(argv[1]));
	}
	else
	{
		fprintf(stderr, "%s: invalid file '%s' at %s, line %d. Must be either a regular "
						"file or a directory", __func__, argv[1], __FILE__, __LINE__);
		return -1;
	}

	if(translate_vm(argv[1], output_path))
	{
		fprintf(stderr, "Failed to translate %s\n", argv[1]);
		return -1;
	}

	return 0;
}
