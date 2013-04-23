#
# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2013)                           *
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
BUILD_LIST+=exprtk_simple_example_01
BUILD_LIST+=exprtk_simple_example_02
BUILD_LIST+=exprtk_simple_example_03
BUILD_LIST+=exprtk_simple_example_04
BUILD_LIST+=exprtk_simple_example_05
BUILD_LIST+=exprtk_simple_example_06
BUILD_LIST+=exprtk_simple_example_07
BUILD_LIST+=exprtk_simple_example_08
BUILD_LIST+=exprtk_simple_example_09
BUILD_LIST+=exprtk_simple_example_10

all: $(BUILD_LIST)

exprtk_test: exprtk_test.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_test exprtk_test.cpp $(LINKER_OPT)

exprtk_benchmark: exprtk_benchmark.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)

exprtk_simple_example_01: exprtk_simple_example_01.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_01 exprtk_simple_example_01.cpp $(LINKER_OPT)

exprtk_simple_example_02: exprtk_simple_example_02.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_02 exprtk_simple_example_02.cpp $(LINKER_OPT)

exprtk_simple_example_03: exprtk_simple_example_03.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_03 exprtk_simple_example_03.cpp $(LINKER_OPT)

exprtk_simple_example_04: exprtk_simple_example_04.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_04 exprtk_simple_example_04.cpp $(LINKER_OPT)

exprtk_simple_example_05: exprtk_simple_example_05.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_05 exprtk_simple_example_05.cpp $(LINKER_OPT)

exprtk_simple_example_06: exprtk_simple_example_06.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_06 exprtk_simple_example_06.cpp $(LINKER_OPT)

exprtk_simple_example_07: exprtk_simple_example_07.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_07 exprtk_simple_example_07.cpp $(LINKER_OPT)

exprtk_simple_example_08: exprtk_simple_example_08.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_08 exprtk_simple_example_08.cpp $(LINKER_OPT)

exprtk_simple_example_09: exprtk_simple_example_09.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_09 exprtk_simple_example_09.cpp $(LINKER_OPT)

exprtk_simple_example_10: exprtk_simple_example_10.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) exprtk_simple_example_10 exprtk_simple_example_10.cpp $(LINKER_OPT)

pgo: exprtk_test.cpp exprtk_benchmark.cpp exprtk.hpp
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-generate -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)
	./exprtk_benchmark
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-use -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)

strip_bin:
	strip -s exprtk_test
	strip -s exprtk_benchmark
	strip -s exprtk_simple_example_01
	strip -s exprtk_simple_example_02
	strip -s exprtk_simple_example_03
	strip -s exprtk_simple_example_04
	strip -s exprtk_simple_example_05
	strip -s exprtk_simple_example_06
	strip -s exprtk_simple_example_07
	strip -s exprtk_simple_example_08
	strip -s exprtk_simple_example_09
	strip -s exprtk_simple_example_10

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_test
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_benchmark
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_01
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_02
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_03
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_04
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_05
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_06
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_07
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_08
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_09
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./exprtk_simple_example_10

clean:
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch
