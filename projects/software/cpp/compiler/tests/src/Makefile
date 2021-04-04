CC = g++
INCLUDE = -I ../../include
LDFLAGS = ../../src/libanalyzer.a -lstdc++fs
CPPFLAGS = -g -Wall --std=c++17

OBJS = test_tokenizer.o test_analyzer.o test_code_generator.o test_heap.o test_symbol_table.o utils.o
APP = tester

.cpp.o:
	@echo "Compiling" $< "..."
	$(CC) -c $(CPPFLAGS) $(INCLUDE) $<

$(APP): $(OBJS)
	@echo "Building" $@ "..."
	make -C ../../src clean
	make -C ../../src
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.o
	rm -rf $(APP)
