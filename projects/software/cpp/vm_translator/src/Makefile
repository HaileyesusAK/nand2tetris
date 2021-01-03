CC = g++
INCLUDE = ../include
CPPFLAGS += -g -Wall --std=c++11
LIB = libvmtranslator.a
OBJS = generator.o translator.o

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
