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
EXE:=count.exe

DATA:=$(wildcard ../data/*.csv)
MY_DATA:=$(DATA:%.csv=%.csv.2)

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(EXE)
	$(QUIET)echo "Close the current plot window, then the next plot will pop up"
	$(QUIET)for t in $(DATA) ; do ./count.exe "$${t}" > "$${t}.2" ; done
	$(QUIET)for t in $(MY_DATA) ; do ../scripts/show_my_csv.py "$${t}"$& ; done


# Sample linking rules for building executables:
count.exe: $(D)/count.o $(D)/stepcounter.o
