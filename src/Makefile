ROOT    :=$(realpath ..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=count.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	#$(QUIET)for t in $^ ; do ./"$$t" || exit 1 ; done
	#$(QUIET)find ../data/*.csv -exec ./count.exe {} as \;
	./count.exe ../data/Andrew_walk_hop_walk_run_stripped.csv

# Sample linking rules for building executables:
count.exe: $(D)/count.o $(D)/stepcounter.o
