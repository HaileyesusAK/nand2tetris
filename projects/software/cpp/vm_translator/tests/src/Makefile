CC = g++
INCLUDE = -I ../../include
LDFLAGS = ../../src/libvmtranslator.a
CPPFLAGS = -g -Wall --std=c++11

OBJS=test_generator.o utils.o
APP=test_generator

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
