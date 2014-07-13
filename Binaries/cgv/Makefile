SOURCES=$(shell echo *.cpp)

LINKED_LIBS= libExec libCallgrindData libProfData libSource\
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 
EXECUTABLE=cgv
MODE=CPP
CPP_TAGS_FILE=profiler-profile-c++.tags
EXTERNAL_LIBS=-lncurses



include ../../makefile.include
