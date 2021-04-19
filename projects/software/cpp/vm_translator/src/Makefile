CC = g++
INCLUDE = ../include
CPPFLAGS += -Wall -Wextra --std=c++17 -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
			-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
			-Wnoexcept -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
			-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
			-Wundef -Werror -Wno-unused
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
