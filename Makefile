#
# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2017)                           *
# * URL: http://www.partow.net/programming/exprtk/index.html   *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the MIT License.                   *
# * http://www.opensource.org/licenses/MIT                     *
# *                                                            *
# **************************************************************
#


COMPILER         = -c++
#COMPILER        = -clang
OPTIMIZATION_OPT = -O1
BASE_OPTIONS     = -pedantic-errors -Wall -Wextra -Werror -Wno-long-long
OPTIONS          = $(BASE_OPTIONS) $(OPTIMIZATION_OPT)
LINKER_OPT       = -L/usr/lib -lstdc++ -lm
ASAN_OPT         = -g -fsanitize=address -fno-omit-frame-pointer
MSAN_OPT         = -g -fsanitize=memory  -fno-omit-frame-pointer
LSAN_OPT         = -g -fsanitize=leak    -fno-omit-frame-pointer

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
BUILD_LIST+=exprtk_simple_example_11
BUILD_LIST+=exprtk_simple_example_12
BUILD_LIST+=exprtk_simple_example_13
BUILD_LIST+=exprtk_simple_example_14
BUILD_LIST+=exprtk_simple_example_15
BUILD_LIST+=exprtk_simple_example_16
BUILD_LIST+=exprtk_simple_example_17
BUILD_LIST+=exprtk_simple_example_18
BUILD_LIST+=exprtk_simple_example_19


all: $(BUILD_LIST)

$(BUILD_LIST) : %: %.cpp exprtk.hpp
	$(COMPILER) $(OPTIONS) -o $@ $@.cpp $(LINKER_OPT)

strip_bin :
	@for f in $(BUILD_LIST); do if [ -f $$f ]; then strip -s $$f; echo $$f; fi done;

valgrind :
	@for f in $(BUILD_LIST); do \
		if [ -f $$f ]; then \
			cmd="valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=$$f.log -v ./$$f"; \
			echo $$cmd; \
			$$cmd; \
		fi done;

pgo: exprtk_benchmark.cpp exprtk.hpp
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-generate -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)
	./exprtk_benchmark
	$(COMPILER) $(BASE_OPTIONS) -O3 -march=native -fprofile-use -o exprtk_benchmark exprtk_benchmark.cpp $(LINKER_OPT)

clean:
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch
