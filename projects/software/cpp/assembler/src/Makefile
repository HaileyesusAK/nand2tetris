CC = g++
INCLUDE = ../include
LDFLAGS += $(CPPUTEST)/lib/libCppUTest.a -lstdc++
CPPFLAGS += -g -Wall --std=c++11
LIB = libassembler.a
OBJS = assembler.o

.cpp.o:
	@echo "Building" $< "..."
	$(CC) -c $(CPPFLAGS) -I $(INCLUDE) $<

install: $(OBJS)
	@echo "Installing" $(LIB)
	ar rc $(LIB) $+
	ranlib $(LIB)

clean:
	rm -rf *.o
	rm -rf $(LIB)
