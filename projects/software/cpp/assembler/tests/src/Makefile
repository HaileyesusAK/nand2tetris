CC = g++
CPPUTEST = $(HOME)/cpputest-3.8
INCLUDE = -I $(CPPUTEST)/include \
		  -I ../../include

LDFLAGS += $(CPPUTEST)/lib/libCppUTest.a \
		   ../../src/libassembler.a
CPPFLAGS += -g -Wall --std=c++11

OBJS=Main.o TestAssembler.o
APP=assembler_tester

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
