CC = g++
INCLUDE = ../include
CPPFLAGS += -Wall --std=c++17
LIB = libanalyzer.a
OBJS = tokenizer.o analyzer.o code_generator.o heap.o symbol_table.o
APP = Analyzer 
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

$(APP): main.o $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@
	
clean:
	rm -rf *.o
	rm -rf $(APP)
	rm -rf $(LIB)
