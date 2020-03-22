#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int assertEqualFile(FILE* f1, FILE* f2, char *msg)
{
	if(!f1 || !f2 || !msg)
	{
		printf("assertEqualFile: error: NULL input pointer(s)\n");
		return 1;
	}

	int rc = 0;

	long f1_size = fseek(f1, 0, SEEK_END);
	rewind(f1);
	
	long f2_size = fseek(f2, 0, SEEK_END);
	rewind(f2);

	if(f1_size != f2_size)
	{
		printf("%li != %li: File size mismatch: %s\n", f1_size, f2_size, msg);
		return 1;
	}
	else
	{
		char* f1_content = malloc(sizeof(char) * f1_size);
		if(!f1_content)
		{
			printf("assertEqualFile: cannot allocate memory for f1 content");
			return 1;
		}
		
		char* f2_content = malloc(sizeof(char) * f2_size);
		if(!f2_content)
		{
			printf("assertEqualFile: cannot allocate memory for f2 content");
			free(f1_content);
			return 1;
		}
		
		fread(f1_content, f1_size, 1, f1);
		fread(f2_content, f2_size, 1, f2);
		if(memcmp(f1_content, f2_content, f1_size))
		{
			printf("File content mismatch: %s\n",  msg);
			rc = 1;
		}

		rewind(f1);
		free(f1_content);
		rewind(f2);
		free(f2_content);
	}

	return rc;
}

int assertEqualInt(int x, int y, char *msg)
{
	if(!msg)
		return 1;

	if(x != y)
	{
		printf("%d != %d: %s\n", x, y, msg);
		return 1;
	}
	return 0;
}

int assertNotNull(void *p, char *msg)
{
	if(!p || !msg)
		return 1;

	if(p == NULL)
	{
		printf("%p == NULL: %s\n", p, msg);
		return 1;
	}
	return 0;
}


int assertEqualString(char *s1, char *s2, char *msg)
{
	if(!s1 || !s2 || !msg)
		return 1;
	
	if(strcmp(s1, s2))
	{
		printf("%s != %s: %s\n", s1, s2, msg);
		return 1;
	}
	return 0;
}


#endif
