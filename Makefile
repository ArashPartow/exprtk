#
# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2012)                           *
# * URL: http://www.partow.net/programming/exprtk/index.html   *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the Common Public License.         *
# * http://www.opensource.org/licenses/cpl1.0.php              *
# *                                                            *
# **************************************************************
#


COMPILER         = -c++
#COMPILER        = -clang
OPTIMIZATION_OPT = -O1
BASE_OPTIONS     = -ansi -pedantic-errors -Wall -Wextra -Werror -Wno-long-long
OPTIONS          = $(BASE_OPTIONS) $(OPTIMIZATION_OPT) -o
LINKER_OPT       = -L/usr/lib -lstdc++

BUILD_LIST+=exprtk_test
BUILD_LIST+=exprtk_benchmark

all: $(BUILD_LIST)

exprtk_test: exprtk_test.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_test exprtk_test.cpp $(LINKER_OPT)

exprtk_benchmark: exprtk_benchmark.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)

pgo: exprtk_test.cpp exprtk_benchmark.cpp exprtk.hpp
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-generate -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)
	./exprtk_benchmark
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-use -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)

strip_bin:
	strip -s exprtk_test
	strip -s exprtk_benchmark

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./exprtk_test
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./exprtk_benchmark

clean:
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch
