CC=gcc
INCPATH=../include
CFLAGS=-g -Wall -I$(INCPATH)
OBJS=main.o hack_interpreter.o parser.o symbol_table.o
APP=assembler

.c.o:
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

$(APP): $(OBJS)
	@echo "Building" $@ "..."
	$(CC) $^ $(CFLAGS) -o $@

HEADERS=$(INCPATH)/hack_interpreter.h $(INCPATH)/parser.h $(INCPATH)/symbol_table.h
$(OBJS): $(HEADERS)

clean:
	rm *.o
	rm $(APP)
