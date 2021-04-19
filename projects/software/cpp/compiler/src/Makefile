CC = g++
INCLUDE = ../include
CPPFLAGS += -Wall -Wextra --std=c++17 -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
			-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
			-Wnoexcept -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
			-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
			-Wundef -Werror -Wno-unused
LIB = libanalyzer.a
OBJS = tokenizer.o analyzer.o code_generator.o heap.o symbol_table.o vm_writer.o
APP = Analyzer 
LDFLAGS = -lstdc++fs

ifeq ($(DEBUG), 1)
CPPFLAGS += -g -DDEBUG
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
