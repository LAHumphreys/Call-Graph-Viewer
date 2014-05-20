LINKED_LIBS= libSource libExec libCallgrindData libProfData \
             libUtils  \
			 libIOInterface \
			 libTest

BUILD_TIME_TESTS=source load path callCount stack profile graph real
CPP_TAGS_FILE=profiler-tests-c++.tags
MODE=CPP

USE_BOOST=YES

NO_INTEL=YES

.TEST_SETUP:
	gcc -g -o toy.out toy.c
	valgrind --tool=callgrind --callgrind-out-file=toy.data --separate-callers=10 ./toy.out

include ../makefile.include
