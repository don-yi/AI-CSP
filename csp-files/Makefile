PRG=gnu.exe
GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++11 -pedantic -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder
DEFINE=-DINLINE_VARIABLE -DINLINE_CONSTRAINT_GRAPH -DINLINE_CONSTRAINT -DINLINE_CSP 

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

DRIVER0=main-online.cpp

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif


gcc0:
	$(GCC) $(DRIVER0) $(CYGWIN) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -o $(PRG)
0:
	@echo "running test$@: 10-queen problem using DFS"
	@echo "should run in less than 1000 ms"
	./$(PRG) $@ >studentout$@ 
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
1:
	@echo "running test$@: 100-queen problem using FC"
	@echo "should run in less than 1000 ms"
	./$(PRG) $@ >studentout$@ 
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
clean:
	rm -f *.exe *.o 
