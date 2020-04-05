CC=gcc
INCPATH=./include
CFLAGS=-g -Wall -I$(INCPATH)
OBJS=vm_translator.o main.o
APP=VMTranslator
VPATH=./src

.c.o:
	@echo "Compiling" $< "..."
	$(CC) -c $(CFLAGS) $<

$(APP): $(OBJS)
	@echo "Building" $@ "..."
	$(CC) $^ $(CFLAGS) -o $@

HEADERS=$(INCPATH)/vm_translator.h
$(OBJS): $(HEADERS)

clean:
	rm -f *.o
	rm -f $(APP)
