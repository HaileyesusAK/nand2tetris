CXX = g++
INCLUDE = include
LDFLAGS = -lstdc++fs
CPPFLAGS =  -Wall -Wextra --std=c++17 -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
			-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
			-Wnoexcept -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
			-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
			-Wundef -Werror -Wno-unused -Wconversion -Wunused-parameter
APP = Assembler
SRC_DIR = src
TEST_DIR = tests
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SOURCES:.cpp=.o)

#TODO: handle HEADERS

$(APP): $(OBJS)
	@echo "linking" $@
	$(CXX) -o $(SRC_DIR)/$@ $^ $(LDFLAGS)

test:
	make -C $(TEST_DIR)

test_clean:
	make -C $(TEST_DIR) clean

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "compiling" $<
	$(CXX) -c $(CPPFLAGS) -I $(INCLUDE) $< -o $@
	@echo ""

clean:
	rm -rf $(OBJS)
	rm -rf $(SRC_DIR)/$(APP)
