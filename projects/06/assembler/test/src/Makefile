CC=gcc
INCPATH=../../include
SRCPATH=../../src

CFLAGS=-g -Wall -I$(INCPATH)
APP=tester
HEADERS=$(INCPATH)/hack_interpreter.h $(INCPATH)/parser.h $(INCPATH)/symbol_table.h
OBJS=hack_interpreter.o parser.o symbol_table.o

$(APP): $(OBJS) tester.c
	@echo "Building" $@ "..."
	$(CC) $^ $(CFLAGS) -o $@

hack_interpreter.o: $(SRCPATH)/hack_interpreter.c $(HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

parser.o: $(SRCPATH)/parser.c $(HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

symbol_table.o: $(SRCPATH)/symbol_table.c $(HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm $(APP)
