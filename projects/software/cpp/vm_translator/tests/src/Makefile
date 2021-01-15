CC = g++
INCLUDE = -I ../../include
LDFLAGS = ../../src/libvmtranslator.a
CPPFLAGS = -g -Wall --std=c++11

OBJS=test_generator.o test_translator.o utils.o
APP=tester

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
	rm -rf ../data/*.out
	rm -rf $(APP)
