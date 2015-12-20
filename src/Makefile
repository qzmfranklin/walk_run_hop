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
	find ../data/*.csv -exec ./count.exe {} 2> /dev/null > {}.2 \;
	find ../data/*.csv.2 -exec ../scripts/show_my_csv.py {} \;

# Sample linking rules for building executables:
count.exe: $(D)/count.o $(D)/stepcounter.o
