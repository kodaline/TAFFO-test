LLVM_DIR		?= $(shell llvm-config --obj-root)
CLANGXX		?= $(LLVM_DIR)/bin/clang++
MAGICLANG		?= ../../magiclang2.sh

ifeq ($(shell $(LLVM_DIR)/bin/llvm-config --build-mode),Debug)
ISDEBUG = 1
$(info debug mode on)
endif
comma :=,

OX          := -O3

CFLAGS		:= -Wall -Wnarrowing -std=c++11 -MD
LFLAGS		+= #-lboost_regex 
HEADERS		:= src
INCLUDE		+= -I${HEADERS} -I../common/src
LIB		:= 

MODULE		:= blackscholes.out

CPP_FILES       := blackscholes.c
OBJ_FILES       := $(addprefix obj/,$(notdir $(CPP_FILES:.c=.o))) 
OBJ_FILES_FIX   := $(addprefix obj/,$(notdir $(CPP_FILES:.c=.fixp.o)))

.PHONY: all clean stats

all: $(MODULE) stats

.PRECIOUS: obj/%.ll obj/%.o obj/%.s obj/%.init.ll obj/%.vra.ll obj/%.tuner.ll obj/%.fixp.ll

DIR:
	@echo ${CPP_FILES}
	@echo ${OBJ_FILES}
	mkdir -p bin
	mkdir -p obj
	mkdir -p stats


$(MODULE): DIR $(OBJ_FILES) $(OBJ_FILES_FIX)
	$(CLANGXX) $(OBJ_FILES) $(LIB) $(LFLAGS) -o bin/$@
	$(CLANGXX) $(OBJ_FILES_FIX) $(LIB) $(LFLAGS) -o bin/$@.fixp

stats: $(MODULE)

obj/%.ll: src/%.c
	$(CLANGXX) -S -emit-llvm $(MIOSIX_FLAGS) $(CFLAGS) $(INCLUDE) -DINPUT=$(INPUT) -DOUTPUT=$(OUTPUT) $< -o $@
	
obj/%.fixp.ll: obj/%.ll
	$(MAGICLANG) -S -emit-llvm -debug -feedback -pe-model $(PE_MODEL) $< -o $@ 2> stats/magiclang2.log

obj/%.s: obj/%.ll
	$(CLANGXX) -S $< -o $@ $(OX)

obj/%.o: obj/%.s
	$(CLANGXX) -c $< -o $@
	
clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf *.out
	rm -rf bin
	rm -rf obj
	rm -rf *.ll
	rm -rf *.s
	rm -rf stats
