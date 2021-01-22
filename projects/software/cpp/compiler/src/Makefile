CC = g++
INCLUDE = ../include
CPPFLAGS += -Wall --std=c++17
OBJS = tokenizer.o main.o
APP = Tokenizer
DEBUG = 1
LDFLAGS = -lstdc++fs

ifeq ($(DEBUG), 1)
CPPFLAGS += -g
endif

.cpp.o:
	@echo "Building" $< "..."
	$(CC) -c $(CPPFLAGS) -I $(INCLUDE) $<

$(APP): $(OBJS)
	@echo "Installing" $@ "..."
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.o
	rm -rf $(LIB)
