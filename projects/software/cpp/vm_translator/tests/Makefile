PROJECT_DIR = ..
SRC_DIR = $(PROJECT_DIR)/src
TEST_DIR = $(PROJECT_DIR)/tests

# where the cpputest library is located
CPPUTEST_HOME = $(HOME)/cpputest-3.8

# specify where the source code and includes are located
INCLUDE_DIRS = $(PROJECT_DIR)/include $(CPPUTEST_HOME)/include
SRC_DIRS = $(PROJECT_DIR)/src

# specify where the test code is located
TEST_SRC_DIRS = $(TEST_DIR)/src

COMPONENT_NAME = vm_translator
CPPUTEST_CXXFLAGS = --std=c++17 -DTEST_DATA_DIR='"$(TEST_DIR)/data"'
CPPUTEST_LDFLAGS = -lstdc++ -lstdc++fs
CPPUTEST_USE_MEM_LEAK_DETECTION = N
CPPUTEST_USE_GCOV = Y

# run MakefileWorker.mk with the variables defined here
include $(CPPUTEST_HOME)/build/MakefileWorker.mk
