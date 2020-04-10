#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>


int assertEqualFile(FILE* f1, FILE* f2, char *msg);

int assertEqualInt(int x, int y, char *msg);

int assertEqualString(char *s1, char *s2, char *msg);

int assertNotNull(void *p, char *msg);


#endif
