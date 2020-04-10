CC=gcc
INCPATH=../../include
SRCPATH=../../src
TESTUTILSPATH=../../../utils

CFLAGS=-g -Wall -I$(INCPATH) 
APP=tester

OBJS=vm_translator.o test_utils.o

$(APP): tester.c $(OBJS)
	@echo "Building" $@ "..."
	$(CC) $(CFLAGS) -I../../../utils $^ -o $@

vm_translator.o: $(SRCPATH)/vm_translator.c $(INCPATH)/vm_translator.h
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

test_utils.o: $(TESTUTILSPATH)/test_utils.c $(TESTUTILSPATH)/test_utils.h
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o
	rm -f $(APP)
	rm -f ../data/result/*
