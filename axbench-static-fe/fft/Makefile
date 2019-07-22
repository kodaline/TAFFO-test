LLVM_DIR		?= $(shell llvm-config --obj-root)
CLANGXX		?= $(LLVM_DIR)/bin/clang++
MAGICLANG		?= ../../magiclang2.sh

ifeq ($(shell $(LLVM_DIR)/bin/llvm-config --build-mode),Debug)
ISDEBUG = 1
$(info debug mode on)
endif
comma :=,

OX          := -O3

CFLAGS		:= -Wall -Wnarrowing -std=c++11
LFLAGS		:= # -lboost_regex 
HEADERS		:= src
INCLUDE 		:= -I${HEADERS} -I../common/src

MODULE		:= fft.out

CPP_FILES       := $(wildcard src/*.cpp)
LL_FILES        := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.ll))) 
OBJ_FLOAT       := obj/main.o
OBJ_FIX         := obj/main.fixp.o

.PHONY: all clean stats

all: $(MODULE) stats

.PRECIOUS: obj/%.ll obj/%.o obj/%.s obj/%.init.ll obj/%.vra.ll obj/%.fixp.ll

DIR:
	@echo ${CPP_FILES}
	mkdir -p bin
	mkdir -p obj
	mkdir -p stats

$(MODULE): DIR $(OBJ_FLOAT) $(OBJ_FIX) 
	$(CLANGXX) $(OBJ_FLOAT) $(LFLAGS) -o bin/$@
	$(CLANGXX) $(OBJ_FIX) $(LFLAGS) -o bin/$@.fixp

stats: $(MODULE) obj/main.err.ll stats/main.fixp.mix.txt stats/main.mix.txt stats/main.llvm.txt stats/main.fixp.mlfeat.txt stats/main.mlfeat.txt


obj/%.ll : src/%.cpp
	$(CLANGXX) -S -emit-llvm $(CFLAGS) $(INCLUDE) $< -o $@

obj/main.ll : $(LL_FILES)
	$(LLVM_DIR)/bin/llvm-link -S $^ -o $@

$(OBJ_FLOAT) : obj/main.ll
	$(CLANGXX) -S $^ -o obj/main.s $(OX)
	$(CLANGXX) -c obj/main.s -o $@

obj/main.fixp.ll : obj/main.ll
	$(MAGICLANG) -S -emit-llvm -debug -feedback -pe-model $(PE_MODEL) $< -o $@ 2> stats/magiclang2.log

$(OBJ_FIX) : obj/main.fixp.ll
	$(CLANGXX) -S $^ -o obj/main.fixp.s $(OX)
	$(CLANGXX) -c obj/main.fixp.s -o $@

clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf *.out
	rm -rf bin
	rm -rf obj
	rm -rf *.ll
	rm -rf *.s
	rm -rf stats
