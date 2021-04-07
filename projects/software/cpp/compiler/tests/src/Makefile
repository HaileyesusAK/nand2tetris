CC = g++
INCLUDE = -I ../../include
LDFLAGS = ../../src/libanalyzer.a -lstdc++fs
CPPFLAGS = -Wall --std=c++17

ifeq ($(DEBUG),)
DEBUG = 0
endif


OBJS = test_tokenizer.o test_analyzer.o test_code_generator.o test_heap.o test_symbol_table.o utils.o
APP = tester

.cpp.o:
	@echo "Compiling" $< "..."
	$(CC) -c $(CPPFLAGS) $(INCLUDE) $<

$(APP): $(OBJS)
	@echo "Building" $@ "..."
	make -C ../../src clean
	make -C ../../src DEBUG=$(DEBUG)
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.o
	rm -rf $(APP)
