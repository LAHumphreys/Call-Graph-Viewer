LINKED_LIBS= libSource libExec libCallgrindData libProfData \
             libUtils  \
			 libString \
			 libIOInterface \
			 libTest
MAKE_DIRS=gcgvApp

BUILD_TIME_TESTS=stack config source load path callCount graph real
CPP_TAGS_FILE=profiler-tests-c++.tags
MODE=CPP

USE_BOOST=YES

NO_INTEL=YES

.TEST_SETUP:
	gcc -g -o toy.out toy.c
	valgrind --tool=callgrind --callgrind-out-file=toy.data --separate-callers=10 ./toy.out
	valgrind --tool=callgrind --callgrind-out-file=toy_cache.data --separate-callers=10 --cache-sim=yes ./toy.out

include ../makefile.include
