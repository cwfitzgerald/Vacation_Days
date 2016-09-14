CC_32     := C:/msys64/mingw32/bin/gcc.exe
CXX_32    := C:/msys64/mingw32/bin/g++.exe
CC_64     := gcc
CXX_64    := g++
CC        := $(CC_64)
CXX       := $(CXX_64)

PROJECT_NAME := Vacation_Time
LIB_NAME     := libvacationdb

OUTPUT_FOLDER := bin/

EXTANT     = _x64.exe

CXXFLAGS  := -fopenmp
WARNINGS  := -Wall -Wextra
FULLWARN  := -Wall -Wextra -Wpedantic
STD       := -std=c++1z 
OPTIMIZE  := -O3
ARCH      := 
DEBUG     := 
DEFINES   := 
INCLUDE32 := 
INCLUDE64 := -Iinclude
LINK      := -lpthread
MODE       = 

MODULES   := 
SRC_DIR   := src $(addprefix src/,$(MODULES))
BUILD_DIR := obj $(addprefix obj/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJN      := $(patsubst src/%.cpp,obj/%.o,$(SRC))
OBJ32     := $(patsubst %.o,%_x86.o, $(OBJN))
OBJ64     := $(patsubst %.o,%_x64.o, $(OBJN))

DLLDIR    := /mingw64/bin/
DLLS      := libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll libgmp-10.dll

LIB_MODULES   := 
LIB_SRC_DIR   := src/$(LIB_NAME) $(addprefix src/$(LIB_NAME)/,$(LIB_MODULES))
LIB_BUILD_DIR := obj/$(LIB_NAME) $(addprefix obj/$(LIB_NAME)/,$(LIB_MODULES))

LIB_SRC       := $(foreach sdir,$(LIB_SRC_DIR),$(wildcard $(sdir)/*.cpp))
LIB_OBJN      := $(patsubst src/%.cpp,obj/%.o,$(LIB_SRC))
LIB_OBJ32     := $(patsubst %.o,%_x86.o, $(LIB_OBJN))
LIB_OBJ64     := $(patsubst %.o,%_x64.o, $(LIB_OBJN))

TESTL_SRC  := $(wildcard src/$(LIB_NAME)/tests/*.cpp)
TESTL_OBJN := $(patsubst src/%.cpp,obj/%_x64.o,$(TESTL_SRC))

.PHONY: all checkdirs clean

all: checkdirs 64bit

multilib: 32bit 64bit

32bit: CC = $(CC_32)
32bit: CXX = $(CXX_32)
32bit: MODE = -m32
32bit: EXTANT = _x86.exe
32bit: INCLUDES := $(INCLUDE32)
32bit: checkdirs bin/$(PROJECT_NAME)_x86.exe

64bit: CC = $(CC_64)
64bit: CXX = $(CXX_64)
64bit: MODE = 
64bit: EXTANT = _x64.exe
64bit: INCLUDES := $(INCLUDE64)
64bit: checkdirs bin/$(PROJECT_NAME)_x64.exe

warning: WARNINGS = $(FULLWARN)
warning: 64bit

debug: DEBUG    = -g
debug: OPTIMIZE = -O0
debug: 64bit
 
test: INCLUDES += -Iinclude/$(LIB_NAME) -Igtest/googletest/include
test: LINK += gtest/googletest/make/gtest_main.a -Lbin/ -lvacationdb
test: CXXFLAGS := 
test: checkdirs bin/$(LIB_NAME)_test.exe 	

dllcopy: 
	@echo Copying $(DLLS)
	@cp -f $(addprefix $(DLLDIR), $(DLLS)) bin


vpath %.cpp $(SRC_DIR)

define make-goal
$1/%_x86.o: %.cpp
	@echo $$(CXX) $$<
	@$$(CXX) $$(WARNINGS) $$(STD) $$(OPTIMIZE) $$(MODE) $$(ARCH) $$(DEBUG) $$(DEFINES) $$(INCLUDES) $$(CXXFLAGS) -c $$< -o $$@

$1/%_x64.o: %.cpp
	@echo $$(CXX) $$<
	@$$(CXX) $$(WARNINGS) $$(STD) $$(OPTIMIZE) $$(MODE) $$(ARCH) $$(DEBUG) $$(DEFINES) $$(INCLUDES) $$(CXXFLAGS) -c $$< -o $$@
endef


bin/$(LIB_NAME).dll: CXXFLAGS += -fPIC
bin/$(LIB_NAME).dll: INCLUDES  = -Iinclude/$(LIB_NAME) -Irapidjson/include
bin/$(LIB_NAME).dll: DEFINES   = -DVACATIONDB_EXPORT	
bin/$(LIB_NAME).dll: LINK      = -lpthread -lgmp 
bin/$(LIB_NAME).dll: $(LIB_OBJ64)
	@echo Creating shared object $@
	@$(CXX) $(DEBUG) $(OPTIMIZE) $(MODE) $(ARCH) $(CXXFLAGS) -shared $^ -o $@ $(LINK)
	

bin/$(PROJECT_NAME)_x86.exe: $(OBJ32) bin/$(LIB_NAME).dll
	@echo Linking $@
	@$(CXX) $(DEBUG) $(OPTIMIZE) $(MODE) $(ARCH) $(CXXFLAGS) $^ -o $@ $(LINK) 

bin/$(PROJECT_NAME)_x64.exe: $(OBJ64) bin/$(LIB_NAME).dll
	@echo Linking $@
	@$(CXX) $(DEBUG) $(OPTIMIZE) $(MODE) $(ARCH) $(CXXFLAGS) $^ -o $@ $(LINK)

bin/$(LIB_NAME)_test.exe: $(TESTL_OBJN) bin/$(LIB_NAME).dll
	@echo Linking $@ 
	@$(CXX) $(DEBUG) $(OPTIMIZE) $(MODE) $(ARCH) $(CXXFLAGS) $^ -o $@ $(LINK)

checkdirs: $(BUILD_DIR) $(LIB_BUILD_DIR) $(TESTL_BUILD_DIR) bin
	

bin:
	@mkdir -p bin

$(BUILD_DIR):
	@mkdir -p $@

$(LIB_BUILD_DIR):
	@mkdir -p $@

$(TESTL_BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -f bin/$(PROJECT_NAME)*.exe bin/$(LIB_NAME)*.dll bin/$(LIB_NAME)_test*.exe
	@find obj/ | awk "/\.o/" | xargs rm -f 

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))