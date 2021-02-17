CC = g++
INCLUDE = ../include
CPPFLAGS += -Wall --std=c++17
LIB = libanalyzer.a
OBJS = tokenizer.o analyzer.o
APP = Tokenizer
DEBUG = 1
LDFLAGS = -lstdc++fs

ifeq ($(DEBUG), 1)
CPPFLAGS += -g
endif

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
