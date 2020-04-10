CC=gcc
INCPATH=../../include
SRCPATH=../../src

CFLAGS=-g -Wall -I$(INCPATH)
APP=tester
SRC_HEADERS=$(INCPATH)/hack_interpreter.h $(INCPATH)/parser.h $(INCPATH)/symbol_table.h

OBJS=parser_tester.o symbol_table_tester.o hack_interpreter_tester.o utils.o\
	 parser.o symbol_table.o hack_interpreter.o 

$(APP): tester.c $(OBJS)
	@echo "Building" $@ "..."
	$(CC) $(CFLAGS) $^ -o $@

hack_interpreter.o: $(SRCPATH)/hack_interpreter.c $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

parser.o: $(SRCPATH)/parser.c $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

symbol_table.o: $(SRCPATH)/symbol_table.c $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

parser_tester.o: parser_tester.c parser.o symbol_table.o parser_tester.h utils.o $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

symbol_table_tester.o: symbol_table_tester.c symbol_table.o symbol_table_tester.h  utils.o $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

hack_interpreter_tester.o: hack_interpreter_tester.c symbol_table.o hack_interpreter.o parser.o hack_interpreter_tester.h utils.o $(SRC_HEADERS)
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

util.o: utils.c utils.h
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
	rm $(APP)
